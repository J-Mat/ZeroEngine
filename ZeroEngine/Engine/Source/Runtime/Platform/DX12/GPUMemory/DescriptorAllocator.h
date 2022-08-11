#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "DescriptorAllocation.h"
#include "DescriptorAllocatorPage.h"

namespace Zero
{
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
		

		FDX12Device& Device;
		D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
		uint32_t NumDescriptorsPerHeap;

		DescriptorHeapPool HeapPool;

		std::set<size_t> AvailableHeaps;
		std::mutex AllocationMutex;
	};
}