#include "DescriptorCache.h"
#include "../../DX12Device.h"

namespace Zero
{
	FDescriptorCache::FDescriptorCache()
	{
		CreateCacheCbvSrvUavDescriptorHeap();

		CreateCacheRtvDescriptorHeap();
	}

	FDescriptorCache::~FDescriptorCache()
	{

	}

	void FDescriptorCache::CreateCacheCbvSrvUavDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = s_MaxCbvSrvUavDescripotrCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		};
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateDescriptorHeap(&SrvHeapDesc, IID_PPV_ARGS(&m_CacheCbvSrvUavDescriptorHeap)));
		m_CacheCbvSrvUavDescriptorHeap->SetName(L"CacheCbvSrvUavDescriptorHeap");

		m_CbvSrvUavDescriptorSize = FDX12Device::Get()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void FDescriptorCache::CreateCacheRtvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = s_MaxCbvSrvUavDescripotrCount,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
		};
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateDescriptorHeap(&RtvHeapDesc, IID_PPV_ARGS(&m_CacheRtvDescriptorHeap)));
		m_CacheRtvDescriptorHeap->SetName(L"CacheRtvDescriptorHeap");

		m_RtvDescriptorSize = FDX12Device::Get()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE FDescriptorCache::AppendCbvSrvUavDescriptors(D3D12_CPU_DESCRIPTOR_HANDLE* DstDescriptor, uint32_t NumDescriptors)
	{
		CORE_ASSERT(m_CbvSrvUavDescriptorOffset + NumDescriptors < s_MaxCbvSrvUavDescripotrCount, "Too Many Descritors!");
		
		auto CpuDescHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CacheCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CbvSrvUavDescriptorOffset, m_CbvSrvUavDescriptorSize);
		FDX12Device::Get()->GetDevice()->CopyDescriptors(1, &CpuDescHandle, &NumDescriptors, NumDescriptors, DstDescriptor, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		auto GpuDescHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_CacheCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_CbvSrvUavDescriptorOffset, m_CbvSrvUavDescriptorSize);

		m_CbvSrvUavDescriptorOffset += NumDescriptors;
		
		return GpuDescHandle;
	}

	void FDescriptorCache::AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle)
	{
		uint32_t SlotNum= (uint32_t)RtvDescriptors.size();
		CORE_ASSERT(m_RtvDescriptorOffset + SlotNum < s_MaxRtvDescriptorCount, "Too Many Descritors!");

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_RtvDescriptorOffset, m_RtvDescriptorSize);
		FDX12Device::Get()->GetDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotNum, SlotNum, RtvDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		OutGpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_CacheRtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_RtvDescriptorOffset, m_RtvDescriptorSize);

		OutCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_RtvDescriptorOffset, m_RtvDescriptorSize);

		// Increase descriptor offset
		m_RtvDescriptorOffset += SlotNum;
	}

	void FDescriptorCache::Reset()
	{
		m_CbvSrvUavDescriptorOffset = 0;
		m_RtvDescriptorOffset = 0;
	}
}
