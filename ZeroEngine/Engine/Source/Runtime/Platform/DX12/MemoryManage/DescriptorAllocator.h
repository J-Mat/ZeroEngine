#pragma once

#include "Core.h"
#include "DescriptorAllocation.h"
#include "../Common/DX12Header.h"
#include "../DX12Device.h"

namespace Zero
{
	//class FDescriptorAllocation;
	class FDescriptorAllocatorPage;
	class FDescriptorAllocator
	{
	public:
		FDescriptorAllocator(FDX12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t InNumDescriptorsPerHeap = 256);
		virtual ~FDescriptorAllocator();
		FDescriptorAllocation Allocate(uint32_t NumDescriptors = 1);
		Ref<FDescriptorAllocatorPage> CreateAllocatorPage();
		void ReleaseStaleDescriptors();
	private:
		using DescriptorHeapPool = std::vector<Ref<FDescriptorAllocatorPage>>;
		

		FDX12Device& m_Device;
		D3D12_DESCRIPTOR_HEAP_TYPE m_HeapType;
		uint32_t m_NumDescriptorsPerHeap;

		DescriptorHeapPool m_HeapPool;

		std::set<size_t> m_AvailableHeaps;
		std::mutex m_AllocationMutex;
	};
}