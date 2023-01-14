#include "DynamicDescriptorHeap.h"
#include "../DX12Device.h"
#include "../DX12CommandList.h"


namespace Zero
{
	FDynamicDescriptorHeap::FDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t NumDescriptorsPerHeap)
		:m_DescriptorHeapType(HeapType)
		,m_NumDescriptorsPerHeap(NumDescriptorsPerHeap)
		,m_DescriptorTableBitMask(0)
		,m_StaleDescriptorTableBitMask(0)
		,m_StaleCBVBitMask(0)
		,m_StaleSRVBitMask(0)
		,m_StaleUAVBitMask(0)	
		,m_CurrentCPUDescriptorHandle(D3D12_DEFAULT)
		,m_CurrentGPUDescriptorHandle(D3D12_DEFAULT)
		,m_NumFreeHandles(0)
	{
		m_DescriptorHandleIncrementSize = FDX12Device::Get()->GetDescriptorHandleIncrementSize(m_DescriptorHeapType);

		// Allocate space for staging CPU visible descriptors.
		m_DescriptorHandleCache = CreateScope<D3D12_CPU_DESCRIPTOR_HANDLE[]>(m_NumDescriptorsPerHeap);
		memset(m_DescriptorHandleCache.get(), 0, m_NumDescriptorsPerHeap);
	}

	FDynamicDescriptorHeap::~FDynamicDescriptorHeap()
	{
	}

	void FDynamicDescriptorHeap::ParseRootSignature(const Ref<FDX12RootSignature>& RootSignature)
	{
		// If the root signature changes, all descriptors must be (re)bound to the
		// command list.
		m_StaleDescriptorTableBitMask = 0;
		
		const auto& RootSignatureDesc = RootSignature->GetRootSignatureDesc_0();

		// Get a bit mask that represents the root parameter indices that match the
		// descriptor heap type for this dynamic descriptor heap.
		m_DescriptorTableBitMask = RootSignature->GetDescriptorTableBitMask(m_DescriptorHeapType);
		uint32_t DescriptorTableBitMask = m_DescriptorTableBitMask;

		uint32_t CurrentOffset = 0;
		DWORD RootIndex;
		
		while (_BitScanForward(&RootIndex, DescriptorTableBitMask) && RootIndex < RootSignatureDesc.NumParameters)
		{
			uint32_t  NumDescriptors = RootSignature->GetNumDescriptors(RootIndex);
			FDescriptorTableCache& DescriptorTableCache = m_DescriptorTableCache[RootIndex];
			DescriptorTableCache.NumDescriptors = NumDescriptors;
			DescriptorTableCache.BaseDescriptor = m_DescriptorHandleCache.get() + CurrentOffset;
			DescriptorTableCache.DescriptorOffset = CurrentOffset;
			
			CurrentOffset += NumDescriptors;

			DescriptorTableBitMask ^= (1 << RootIndex);
		}
		CORE_ASSERT(CurrentOffset <= m_NumDescriptorsPerHeap,
			"The root signature requires more than the maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap.");
	}

	void FDynamicDescriptorHeap::CommitStagedDescriptorsForDraw(FDX12CommandList& CommandList)
	{
		CommitDescriptorTables(CommandList, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
		SetAsShaderResourceHeap();
	}

	ComPtr<ID3D12DescriptorHeap> FDynamicDescriptorHeap::RequestDescriptorHeap()
	{
		ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
		if (!m_AvailableDescriptorHeaps.empty())
		{
			DescriptorHeap = m_AvailableDescriptorHeaps.front();
			m_AvailableDescriptorHeaps.pop();
		}
		else
		{
			DescriptorHeap = CreateDescriptorHeap();
			m_DescriptorHeapPool.push(DescriptorHeap);
		}
		
		return DescriptorHeap;
	}

	ComPtr<ID3D12DescriptorHeap> FDynamicDescriptorHeap::CreateDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
		Desc.Type = m_DescriptorHeapType;
		Desc.NumDescriptors = m_NumDescriptorsPerHeap;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
		ThrowIfFailed(
			FDX12Device::Get()->GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&DescriptorHeap))
		);
	
		return DescriptorHeap;
	}

	uint32_t FDynamicDescriptorHeap::ComputeStaleDescriptorCount() const
	{
		uint32_t NumStaleDescriptors = 0;
		DWORD RootIndex = 0;
		DWORD StaleDescriptorsBitMask = m_StaleDescriptorTableBitMask;
		
		while (_BitScanForward(&RootIndex, StaleDescriptorsBitMask))
		{
			NumStaleDescriptors += m_DescriptorTableCache[RootIndex].NumDescriptors;
			StaleDescriptorsBitMask ^= (1 << RootIndex);
		}
		
		return NumStaleDescriptors;
	}

	void FDynamicDescriptorHeap::CommitDescriptorTables(FDX12CommandList& CommandList, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> SetFunc)

	{
		uint32_t NumDescriptorsToCommit = ComputeStaleDescriptorCount();
		if (NumDescriptorsToCommit > 0)
		{
			if (!m_CurrentDescriptorHeap)
			{
				m_CurrentDescriptorHeap = RequestDescriptorHeap();
				m_NumFreeHandles = m_NumDescriptorsPerHeap;
				
				CommandList.SetDescriptorHeap(m_DescriptorHeapType, m_CurrentDescriptorHeap.Get());

				// When updating the descriptor heap on the command list, all descriptor
				// tables must be (re)recopied to the new descriptor heap (not just
				// the stale descriptor tables).
				m_StaleDescriptorTableBitMask = m_DescriptorTableBitMask;
			}
		}
		else
		{
			return;
		}
		CommandList.SetDescriptorHeap(m_DescriptorHeapType, m_CurrentDescriptorHeap.Get());

		DWORD RootIndex;
		// Scan from LSB to MSB for a bit set in staleDescriptorsBitMask
		while (_BitScanForward(&RootIndex, m_StaleDescriptorTableBitMask))
		{
			m_CurrentCPUDescriptorHandle = m_CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			m_CurrentGPUDescriptorHandle = m_CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

			UINT DescriptorOffset = m_DescriptorTableCache[RootIndex].DescriptorOffset;
			m_CurrentCPUDescriptorHandle.Offset(DescriptorOffset, m_DescriptorHandleIncrementSize);
			m_CurrentGPUDescriptorHandle.Offset(DescriptorOffset, m_DescriptorHandleIncrementSize);

			UINT NumSrcDescriptors = m_DescriptorTableCache[RootIndex].NumDescriptors;
			D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorHandles = m_DescriptorTableCache[RootIndex].BaseDescriptor;
			
			D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[] = { m_CurrentCPUDescriptorHandle };
			UINT pDestDescriptorRangeSizes[] = {NumSrcDescriptors};

			if (pSrcDescriptorHandles->ptr)
			{
				// Copy the staged CPU visible descriptors to the GPU visible descriptor heap.
				FDX12Device::Get()->GetDevice()->CopyDescriptors(1, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, NumSrcDescriptors,
					pSrcDescriptorHandles, nullptr, m_DescriptorHeapType);

				// Set the descriptors on the command list using the passed-in setter function.
				SetFunc(CommandList.GetD3D12CommandList().Get(), RootIndex, m_CurrentGPUDescriptorHandle);
			}

			// Offset current CPU and GPU descriptor handles.

			// Flip the stale bit so the descriptor table is not recopied again unless it is updated with a new
			// descriptor.
			m_StaleDescriptorTableBitMask ^= (1 << RootIndex);
		}
	}

	void FDynamicDescriptorHeap::CommitInlineDescriptors(FDX12CommandList& CommandList, const D3D12_GPU_VIRTUAL_ADDRESS* BufferLocations, uint32_t& BitMask, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_VIRTUAL_ADDRESS)> SetFunc)
	{
		if (BitMask != 0)
		{
			DWORD RootIndex; 
			while (_BitScanForward(&RootIndex, BitMask))
			{
				SetFunc(CommandList.GetD3D12CommandList().Get(), RootIndex, BufferLocations[RootIndex]);
				
				BitMask ^= (1 << RootIndex);
			}
		}
	}

	void FDynamicDescriptorHeap::StageDescriptors(uint32_t RootParameterIndex, uint32_t Offset, uint32_t NumDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE SrcDescriptors) 
	{ 
		if (NumDescriptors > m_NumDescriptorsPerHeap || RootParameterIndex > s_MaxDescriptorTables)
		{
			throw std::bad_alloc();
		}

		FDescriptorTableCache& DescriptorTableCache = m_DescriptorTableCache[RootParameterIndex];

		if ((Offset + NumDescriptors) > DescriptorTableCache.NumDescriptors)
		{
			throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
		}
		D3D12_CPU_DESCRIPTOR_HANDLE* DstDescriptor = (DescriptorTableCache.BaseDescriptor + Offset);
		for (uint32_t i = 0; i < NumDescriptors; ++i)
		{
			DstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(SrcDescriptors, i, m_DescriptorHandleIncrementSize);
		}
		
		m_StaleDescriptorTableBitMask |= (1 << RootParameterIndex);
	}

	void FDynamicDescriptorHeap::SetAsShaderResourceHeap()
	{ 
		if (!m_CurrentDescriptorHeap)
		{
			return;
		}
		uint32_t TableBitMask = m_DescriptorTableBitMask;
		DWORD RootIndex;
		
		auto CommandList = FDX12Device::Get()->GetRenderCommandList();
		CommandList->SetDescriptorHeap(m_DescriptorHeapType,  m_CurrentDescriptorHeap.Get());

		m_CurrentCPUDescriptorHandle = m_CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_CurrentGPUDescriptorHandle = m_CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		if (m_CurrentDescriptorHeap != nullptr)
		{
			// Scan from LSB to MSB for a bit set in staleDescriptorsBitMask
			while (_BitScanForward(&RootIndex, TableBitMask))
			{
				UINT NumSrcDescriptors = m_DescriptorTableCache[RootIndex].NumDescriptors;
				if (m_DescriptorTableCache[RootIndex].BaseDescriptor->ptr)
				{
					CommandList->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(RootIndex, m_CurrentGPUDescriptorHandle);
				}
				m_CurrentCPUDescriptorHandle.Offset(NumSrcDescriptors, m_DescriptorHandleIncrementSize);
				m_CurrentGPUDescriptorHandle.Offset(NumSrcDescriptors, m_DescriptorHandleIncrementSize);
				TableBitMask ^= (1 << RootIndex);
			}
		}
	}

	void FDynamicDescriptorHeap::StageInlineCBV(uint32_t RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		m_InlineCBV[RootParameterIndex] = BufferLocation;
		m_StaleCBVBitMask |= (1 << RootParameterIndex);
	}

	void FDynamicDescriptorHeap::StageInlineSRV(uint32_t RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		m_InlineSRV[RootParameterIndex] = BufferLocation;
		m_StaleSRVBitMask |= (1 << RootParameterIndex);
	}

	void FDynamicDescriptorHeap::StageInlineUAV(uint32_t RootParamterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		m_InlineUAV[RootParamterIndex] = BufferLocation;
		m_StaleUAVBitMask |= (1 << RootParamterIndex);
	}


	void FDynamicDescriptorHeap::Reset()
	{
		m_AvailableDescriptorHeaps = m_DescriptorHeapPool;
		m_CurrentDescriptorHeap.Reset();
		m_CurrentCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
		m_CurrentGPUDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
		m_NumFreeHandles = 0;
		m_DescriptorTableBitMask = 0;
		m_StaleDescriptorTableBitMask = 0;
		m_StaleCBVBitMask = 0;
		m_StaleSRVBitMask = 0;
		m_StaleUAVBitMask = 0;

		// Reset the descriptor cache
		for (int i = 0; i < s_MaxDescriptorTables; ++i)
		{
			m_DescriptorTableCache[i].Reset();
			m_InlineCBV[i] = 0ull;
			m_InlineSRV[i] = 0ull;
			m_InlineUAV[i] = 0ull;
		}
	}
}
