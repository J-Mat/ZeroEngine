#include "UploadBuffer.h"

namespace Zero
{
	FUploadBuffer::FUploadBuffer(FDX12Device& Device, size_t PageSize)
	: m_Device(Device)
	, m_PageSize(PageSize)
	{
				
	}

	FUploadBuffer::FPage::FPage(FDX12Device& InDevice, size_t InSizeInBytes)
		: m_Device(InDevice)
		, m_PageSize(InSizeInBytes)
		, m_Offset(0)
		, m_CPUPtr(nullptr)
		, m_GPUPtr(D3D12_GPU_VIRTUAL_ADDRESS(0))
	{
		ThrowIfFailed(
			m_Device.GetDevice()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_PageSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				IID_PPV_ARGS(&Resource))
		);
		
		Resource->SetName(L"Upload Buffer (Page)");
		
		m_GPUPtr = Resource->GetGPUVirtualAddress();
		Resource->Map(0, nullptr, &m_CPUPtr);
	}

	FUploadBuffer::FPage::~FPage()
	{
		Resource->Unmap(0, nullptr);
		m_CPUPtr = nullptr;
		m_GPUPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	bool FUploadBuffer::FPage::HasSpace(size_t SizeInBytes, size_t Alignment) const
	{
		size_t AlignedSize = Math::AlignUp(SizeInBytes, Alignment);
		size_t AlignedOffset = Math::AlignUp(m_Offset, Alignment);
		return AlignedOffset + AlignedSize <= m_PageSize;
	}

	FUploadBuffer::FAllocation FUploadBuffer::FPage::Allocate(size_t SizeInBytes, size_t Alignment)
	{
		if (!HasSpace(SizeInBytes, Alignment))
		{
			throw std::bad_alloc();
		}
		size_t AlignedSize = Math::AlignUp(SizeInBytes, Alignment);
		m_Offset = Math::AlignUp(m_Offset, Alignment);
		
		FAllocation Allocation = { static_cast<uint8_t*>(m_CPUPtr) + m_Offset, m_GPUPtr + m_Offset };
		
		m_Offset += AlignedSize;
		
		return Allocation;
	}



	FUploadBuffer::FAllocation FUploadBuffer::Allocate(size_t SizeInBytes, size_t Alignment)
	{
		if (SizeInBytes > m_PageSize)
		{
			throw std::bad_alloc();
		}

		if (!m_CurrentPage || m_CurrentPage->HasSpace(SizeInBytes, Alignment))
		{
			m_CurrentPage = RequestPage();
		}
		return m_CurrentPage->Allocate(SizeInBytes, Alignment);
	}

	void FUploadBuffer::Reset()
	{
		m_CurrentPage = nullptr;
		m_AvaliablePages = m_PagePool;
		for (auto Page : m_AvaliablePages)
		{
			Page->Reset();
		}
	}

	Ref<FUploadBuffer::FPage> FUploadBuffer::RequestPage()
	{
		Ref<FUploadBuffer::FPage> Page;
		
		if (!m_AvaliablePages.empty())
		{
			Page = m_AvaliablePages.front();
			m_AvaliablePages.pop_front();
		}
		else
		{
			Page = CreateRef<FUploadBuffer::FPage>(m_Device, m_PageSize);
			m_PagePool.push_back(Page);
		}

		return Page;
	}

	void FUploadBuffer::FPage::Reset()
	{
		m_Offset = 0;
	}
}