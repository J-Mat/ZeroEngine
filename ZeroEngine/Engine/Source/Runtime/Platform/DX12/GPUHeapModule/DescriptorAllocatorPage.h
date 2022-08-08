#include "Core.h"
#include "Common/DX12Header.h"
#include "DX12Device.h"


namespace Zero
{
	class FDescriptorAllocatorPage : public std::enable_shared_from_this<FDescriptorAllocatorPage>
	{
	public:
		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const {return HeapType;}
		FDescriptorAllocatorPage(FDX12Device& Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptors);

		void AddNewBlock(uint32_t Offset, uint32_t NumDescriptors);
		void FreeBlock(uint32_t Offset, uint32_t NumDescriptors);
	private:
		struct FFreeBlockInfo;

		using FFreeListByOffset = std::map<uint32_t, FFreeBlockInfo>;
		using FFreeListBySize = std::multimap<uint32_t, FFreeListByOffset::iterator>;

		struct FFreeBlockInfo
		{
			FFreeBlockInfo(uint32_t InSize)
			: Size(InSize) 
			{}

			uint32_t Size;
			FFreeListBySize::iterator FreeListBySizeIter;
		};


		struct StaleDescriptorInfo
    	{
			StaleDescriptorInfo( OffsetType InOffset, SizeType InSize )
        	: Offset( InOffset )
        	, Size( InSize )
        	{}
   			// The InOffset within the descriptor heap.
        	uint32_t Offset;
        	// The number of descriptors
        	uint32_t Size;
    	};

		using FStaleDescriptorQueue = std::queue<FStaleDescriptorInfo>;

		FFreeListByOffset FreeListByOffset;
		FFreeListBySize  FreeListBySize;
		FStaleDescriptorQueue StaleDescriptorQueue;

		D3D12_DESCRIPTOR_HEAP_TYPE HeapType;

		FDX12Device& Device;

  		ComPtr<ID3D12DescriptorHeap> D3DDescriptorHeap;
		D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE BaseDescriptor;
		uint32_t  DescriptorHandleIncrementSize;
		uint32_t  NumDescriptorsInHeap;
		uint32_t  NumFreeHandles;
		std::mutex AllocationMutex;
	}

}