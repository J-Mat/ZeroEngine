#pragma once
#include "Core.h"
#include "Platform/DX12/Common/DX12Header.h"


namespace Zero
{
	class FDescriptorCache
	{
	public:
		FDescriptorCache();
		~FDescriptorCache();

		ComPtr<ID3D12DescriptorHeap> GetCacheCbvSrvUavDescriptorHeap() { return m_CacheCbvSrvUavDescriptorHeap; }

		ComPtr<ID3D12DescriptorHeap> GetCacheRtvDescriptorHeap() { return m_CacheRtvDescriptorHeap; }

		CD3DX12_GPU_DESCRIPTOR_HANDLE AppendCbvSrvUavDescriptors(D3D12_CPU_DESCRIPTOR_HANDLE* DstDescriptor, uint32_t NumDescriptors);

		void AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle);

		void Reset();
	private: 
		void CreateCacheCbvSrvUavDescriptorHeap();

		void CreateCacheRtvDescriptorHeap();


	private:
		static const int s_MaxCbvSrvUavDescripotrCount = 2048;
		static const int s_MaxRtvDescriptorCount = 1024;
		ComPtr<ID3D12DescriptorHeap> m_CacheCbvSrvUavDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> m_CacheRtvDescriptorHeap;
		UINT m_CbvSrvUavDescriptorSize;
		UINT m_RtvDescriptorSize;
		uint32_t m_CbvSrvUavDescriptorOffset = 0;
		uint32_t m_RtvDescriptorOffset = 0;
	};
}
