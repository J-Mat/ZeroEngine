#include "UploadBuffer.h"

namespace Zero
{
	FUploadBuffer::FPage::FPage(FDX12Device& InDevice, size_t InSizeInBytes)
		: Device(InDevice)
		, PageSize(InSizeInBytes)
		, Offset(0)
		, CPUPtr(nullptr)
		, GPUPtr(D3D12_GPU_VIRTUAL_ADDRESS(0))
	{
		ThrowIfFailed(
			Device.GetDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(PageSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				IID_PPV_ARGS(&Resource))
		);
		
		Resource->SetName(L"Upload Buffer (Page)");
		
		GPUPtr = Resource->GetGPUVirtualAddress();
		Resource->Map(0, nullptr, &CPUPtr);
	}

	FUploadBuffer::FPage::~FPage()
	{
		Resource->Unmap(0, nullptr);
		CPUPtr = nullptr;
		GPUPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	bool FUploadBuffer::FPage::HasSpace(size_t SizeInBytes, size_t Alignment) const
	{
		size_t AlignedSize = Math::AlignUp(SizeInBytes, Alignment);
		size_t AlignedOffset = Math::AlignUp(Offset, Alignment);
		return AlignedOffset + AlignedSize <= PageSize;
	}

	FUploadBuffer::FAllocation FUploadBuffer::FPage::Allocate(size_t SizeInBytes, size_t Alignment)
	{
		if (!HasSpace(SizeInBytes, Alignment))
		{
			throw std::bad_alloc();
		}
		size_t AlignedSize = Math::AlignUp(SizeInBytes, Alignment);
		Offset = Math::AlignUp(Offset, Alignment);
		
		FAllocation Allocation = { static_cast<uint8_t*>(CPUPtr) + Offset, GPUPtr + Offset };
		
		Offset += AlignedSize;
		
		return Allocation;
	}


	FUploadBuffer::FAllocation FUploadBuffer::Allocate(size_t SizeInBytes, size_t Alignment)
	{
		if (SizeInBytes > PageSize)
		{
			throw std::bad_alloc();
		}

		if (!CurrentPage || CurrentPage->HasSpace(SizeInBytes, Alignment))
		{
			CurrentPage = RequestPage();
		}
		return CurrentPage->Allocate(SizeInBytes, Alignment);
	}

	void FUploadBuffer::Reset()
	{
		CurrentPage = nullptr;
		AvaliablePages = PagePool;
		for (auto Page : AvaliablePages)
		{
			Page->Reset();
		}
	}

	Ref<FUploadBuffer::FPage> FUploadBuffer::RequestPage()
	{
		Ref<FUploadBuffer::FPage> Page;
		
		if (!AvaliablePages.empty())
		{
			Page = AvaliablePages.front();
			AvaliablePages.pop_front();
		}
		else
		{
			Page = CreateRef<FUploadBuffer::FPage>(Device, PageSize);
			PagePool.push_back(Page);
		}

		return Page;
	}

	void FUploadBuffer::FPage::Reset()
	{
		Offset = 0;
	}
}