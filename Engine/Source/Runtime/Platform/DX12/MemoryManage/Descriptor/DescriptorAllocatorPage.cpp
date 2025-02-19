#include "DescriptorAllocatorPage.h"
#include "../../DX12Device.h"
#include "DescriptorAllocation.h"


namespace Zero
{
	FDescriptorAllocatorPage::FDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE Type, 
													  uint32_t NumDescriptors)
	: m_HeapType(Type)
	, m_NumDescriptorsInHeap(NumDescriptors)
	{
		auto D3dDevice = FDX12Device::Get()->GetDevice();
		
		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
		HeapDesc.Type   = m_HeapType;
		HeapDesc.NumDescriptors = m_NumDescriptorsInHeap;

		ThrowIfFailed(
			D3dDevice->CreateDescriptorHeap(&HeapDesc,  IID_PPV_ARGS( &m_D3DDescriptorHeap ) )
		);
		m_BaseDescriptor = m_D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		
		m_DescriptorHandleIncrementSize = D3dDevice->GetDescriptorHandleIncrementSize(m_HeapType);
		m_NumFreeHandles = m_NumDescriptorsInHeap;
		
		AddNewBlock(0, m_NumFreeHandles);
	}
	
	
	bool FDescriptorAllocatorPage::HasSpace(uint32_t NumDescriptors)
	{
		return m_FreeListBySize.lower_bound(NumDescriptors) != m_FreeListBySize.end();
	}

	uint32_t FDescriptorAllocatorPage::GetNumFreeHandles() const
	{
		return m_NumFreeHandles;
	}

	void FDescriptorAllocatorPage::AddNewBlock(uint32_t Offset, uint32_t NumDescriptors)
	{
		auto OffsetIter = m_FreeListByOffset.emplace(Offset, NumDescriptors).first;
		auto SizeIter = m_FreeListBySize.emplace(NumDescriptors, OffsetIter);
		OffsetIter->second.FreeListBySizeIter = SizeIter;
	}

	void FDescriptorAllocatorPage::FreeBlock(uint32_t Offset, uint32_t NumDescriptors)
	{
		// Find the first element whose offset is greater than the specified offset.
	    // This is the block that should appear after the block that is being freed.
		auto NextBlockIter = m_FreeListByOffset.upper_bound(Offset);
		
		// Find the block that appears before the block being freed.
		auto PreBlockIter = NextBlockIter;
		if (NextBlockIter != m_FreeListByOffset.begin())
		{
			--PreBlockIter;
		}
		else
		{
			PreBlockIter = m_FreeListByOffset.end();
		}
		

		// Add the number of free handles back to the heap.
		// This needs to be done before merging any blocks since merging
		// blocks modifies the numDescriptors variable.
		m_NumFreeHandles += NumDescriptors;
		
		if (PreBlockIter != m_FreeListByOffset.end() && Offset == PreBlockIter->first + PreBlockIter->second.Size)
		{
			// The previous block is exactly behind the block that is to be freed.
			//
			// PrevBlock.Offset           Offset
			// |                          |
			// |<-----PrevBlock.Size----->|<------Size-------->|

			 // Increase the block size by the size of merging with the previous block.

			Offset = PreBlockIter->first;
			NumDescriptors += PreBlockIter->second.Size;

			// Remove the previous block from the free list.
			m_FreeListBySize.erase(PreBlockIter->second.FreeListBySizeIter);
			m_FreeListByOffset.erase(PreBlockIter);
		}

		if (NextBlockIter != m_FreeListByOffset.end() && Offset + NumDescriptors == NextBlockIter->first)
		{
			// The next block is exactly in front of the block that is to be freed.
			//
			// Offset               NextBlock.Offset
			// |                    |
			// |<------Size-------->|<-----NextBlock.Size----->|

			// Increase the block size by the size of merging with the next block.
			
			NumDescriptors += NextBlockIter->second.Size;

			// Remove the next block from the free list.
			m_FreeListBySize.erase(NextBlockIter->second.FreeListBySizeIter);
			m_FreeListByOffset.erase(NextBlockIter);
		}

		AddNewBlock(Offset, NumDescriptors);
	}

	void FDescriptorAllocatorPage::Free(FDescriptorAllocation&& m_Descriptor)
	{
		uint32_t Offset = ComputeOffset(m_Descriptor.GetDescriptorHandle());
		
		std::lock_guard<std::mutex> Lock(m_AllocationMutex);

		m_StaleDescriptorQueue.emplace( Offset, m_Descriptor.GetNumHandles() );
	}


	uint32_t FDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
	{
		return static_cast<uint32_t>(Handle.ptr - m_BaseDescriptor.ptr) / m_DescriptorHandleIncrementSize;
	}

	FDescriptorAllocation FDescriptorAllocatorPage::Allocate(uint32_t NumDescriptors)
	{
		std::lock_guard<std::mutex> Lock(m_AllocationMutex);
		
		if (NumDescriptors > m_NumFreeHandles)
		{
			return FDescriptorAllocation();
		}
		
		auto SmallestBlockIter = m_FreeListBySize.lower_bound(NumDescriptors);
		if (SmallestBlockIter == m_FreeListBySize.end())
		{
			return FDescriptorAllocation();
		}
		
		// The size of the smallest block that satisfies the request.
		uint32_t BlockSize = SmallestBlockIter->first;

		// The pointer to the same entry in the FreeListByOffset map.
		auto OffsetIter = SmallestBlockIter->second;

		// The offset in the descriptor heap.
		UINT32 Offset = OffsetIter->first;

		// Remove the existing free block from the free list.
		m_FreeListBySize.erase(SmallestBlockIter);
		m_FreeListByOffset.erase(OffsetIter);


		auto NewOffset = Offset + NumDescriptors;
		auto NewSize = BlockSize - NumDescriptors;

		if (NewSize > 0)
		{
			AddNewBlock(NewOffset, NewSize);
		}
	
		m_NumFreeHandles -= NumDescriptors;
		return FDescriptorAllocation(CD3DX12_CPU_DESCRIPTOR_HANDLE(m_BaseDescriptor, Offset, m_DescriptorHandleIncrementSize),
			NumDescriptors, m_DescriptorHandleIncrementSize, shared_from_this());
	}

	void FDescriptorAllocatorPage::ReleaseStaleDescriptors()
	{
		std::lock_guard<std::mutex> Lock(m_AllocationMutex);
		
		while (!m_StaleDescriptorQueue.empty())
		{
			auto& StaleDescriptor = m_StaleDescriptorQueue.front();
			uint32_t Offset = StaleDescriptor.Offset;
			uint32_t Size = StaleDescriptor.Size;
			FreeBlock(Offset, Size);

			m_StaleDescriptorQueue.pop();
		}
	}
}
