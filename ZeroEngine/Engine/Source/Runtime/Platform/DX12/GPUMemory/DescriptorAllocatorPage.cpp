#include "DescriptorAllocatorPage.h"


namespace Zero
{
	FDescriptorAllocatorPage::FDescriptorAllocatorPage(FDX12Device& InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, 
													  uint32_t NumDescriptors)
	: Device(InDevice)
	, HeapType(Type)
	, NumDescriptorsInHeap(NumDescriptors)
	{
		auto D3dDevice = Device.GetDevice();
		
		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
		HeapDesc.Type   = HeapType;
		HeapDesc.NumDescriptors = NumDescriptorsInHeap;

		ThrowIfFailed(
			D3dDevice->CreateDescriptorHeap(HeapDesc,  IID_PPV_ARGS( &D3DDescriptorHeap ) )
		);
		
		BaseDescriptor = D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		DescriptorHandleIncrementSize = D3dDevice->GetDescriptorHandleIncrementSize(HeapType);
		NumFreeHandles = NumDescriptorsInHeap;
		
		AddNewBlock(0, NumFreeHandles);
	}
	
	
	bool FDescriptorAllocatorPage::HasSpace(uint32_t NumDescriptors)
	{
		return FreeListBySize.lower_bound(NumDescriptors) != FreeListBySize.end();
	}

	uint32_t FDescriptorAllocatorPage::GetNumFreeHandles() const
	{
		return NumFreeHandles;
	}

	void FDescriptorAllocatorPage::AddNewBlock(uint32_t Offset, uint32_t NumDescriptors)
	{
		auto OffsetIter = FreeListByOffset.emplace(Offset, NumDescriptors).first;
		auto SizeIter = FreeListBySize.emplace(NumDescriptors, OffsetIter);
		OffsetIter->second.FreeListBySizeIter = SizeIter;
	}

	void FDescriptorAllocatorPage::FreeBlock(uint32_t Offset, uint32_t NumDescriptors)
	{
		// Find the first element whose offset is greater than the specified offset.
	    // This is the block that should appear after the block that is being freed.
		auto NextBlockIter = FreeListByOffset.upper_bound(Offset);
		auto PreBlockIter = NextBlockIter;
		if (NextBlockIter != FreeListByOffset.begin())
		{
			--PreBlockIter;
		}
		else
		{
			PreBlockIter = FreeListByOffset.end();
		}
		
		NumFreeHandles += NumDescriptors;
		
		if (PreBlockIter != FreeListByOffset.end() && Offset == PreBlockIter->first + PreBlockIter->second.Size)
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
			FreeListBySize.erase(PreBlockIter->second.FreeListBySizeIter);
			FreeListByOffset.erase(PreBlockIter);
		}

		if (NextBlockIter != FreeListByOffset.end() && Offset + NumDescriptors == NextBlockIter->first)
		{
			// The next block is exactly in front of the block that is to be freed.
			//
			// Offset               NextBlock.Offset
			// |                    |
			// |<------Size-------->|<-----NextBlock.Size----->|

			// Increase the block size by the size of merging with the next block.
			
			NumDescriptors += NextBlockIter->second.Size;

			// Remove the next block from the free list.
			FreeListBySize.erase(NextBlockIter->second.FreeListBySizeIter);
			FreeListByOffset.erase(NextBlockIter);
		}

		AddNewBlock(Offset, NumDescriptors);
	}

	void FDescriptorAllocatorPage::Free(FDescriptorAllocation&& Descriptor)
	{
		uint32_t Offset = ComputeOffset(Descriptor.GetDescriptorHandle());
		
		std::lock_guard<std::mutex> Lock(AllocationMutex);

		StaleDescriptorQueue.emplace( Offset, Descriptor.GetNumHandles() );
	}


	uint32_t FDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
	{
		return static_cast<uint32_t>(Handle.ptr - BaseDescriptor.ptr) / DescriptorHandleIncrementSize;
	}

	FDescriptorAllocation FDescriptorAllocatorPage::Allocate(uint32_t NumDescriptors)
	{
		std::lock_guard<std::mutex> Lock(AllocationMutex);
		
		if (NumDescriptors > NumFreeHandles)
		{
			return FDescriptorAllocation();
		}
		
		auto SmallestBlockIter = FreeListBySize.lower_bound(NumDescriptors);
		if (SmallestBlockIter == FreeListBySize.end())
		{
			return FDescriptorAllocation();
		}
		
		uint32_t BlockSize = SmallestBlockIter->first;
		auto OffsetIter = SmallestBlockIter->second;
		UINT32 Offset = OffsetIter->first;

		auto NewOffset = Offset + NumDescriptors;
		auto NewSize = BlockSize - NumDescriptors;

		if (NewSize > 0)
		{
			AddNewBlock(NewOffset, NewSize);
		}
	
		NumFreeHandles -= NumDescriptors;
		return { CD3DX12_CPU_DESCRIPTOR_HANDLE(BaseDescriptor, Offset, DescriptorHandleIncrementSize), NumDescriptors, DescriptorHandleIncrementSize, shared_from_this() };
	}

	void FDescriptorAllocatorPage::ReleaseStaleDescriptors()
	{
		std::lock_guard<std::mutex> Lock(AllocationMutex);
		
		while (!StaleDescriptorQueue.empty())
		{
			auto& StaleDescriptor = StaleDescriptorQueue.front();
			uint32_t Offset = StaleDescriptor.Offset;
			uint32_t Size = StaleDescriptor.Size;
			FreeBlock(Offset, Size);

			StaleDescriptorQueue.pop();
		}
	}
}
