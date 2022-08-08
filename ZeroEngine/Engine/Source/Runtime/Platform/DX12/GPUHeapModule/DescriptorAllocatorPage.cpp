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
	}
	
	void FDescriptorAllocatorPage::AddNewBlock(uint32_t Offset, uint32_t NumDescriptors)
	{
		auto OffsetIter = FreeListByOffset.emplace()
	}

	void FDescriptorAllocatorPage::FreeBlock(uint32_t Offset, uint32_t NumDescriptors);
	{
	}
}
