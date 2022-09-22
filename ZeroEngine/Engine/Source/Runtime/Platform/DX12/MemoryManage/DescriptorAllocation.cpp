#include "DescriptorAllocation.h"
#include "../DX12Device.h"
#include "DescriptorAllocatorPage.h" 


namespace Zero
{
	FDescriptorAllocation::FDescriptorAllocation()
		: m_Descriptor{ 0 }
		, m_NumHandles(0)
		, DescriptorSize(0)
		, Page(nullptr)
	{
	}

	FDescriptorAllocation::FDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle, uint32_t InNumHandles, uint32_t InDescriptorSize, Ref<FDescriptorAllocatorPage> InPage)
		: m_Descriptor(InDescriptor)
		, m_GpuHandle(GpuHandle)
		, m_NumHandles(InNumHandles)
		, DescriptorSize(InDescriptorSize)
		, Page(InPage)
	{
	}

	FDescriptorAllocation::~FDescriptorAllocation()
	{
		Free();
	}
	
	FDescriptorAllocation::FDescriptorAllocation(FDescriptorAllocation&& Allocation) noexcept
		: m_Descriptor(Allocation.m_Descriptor)
		, m_GpuHandle(Allocation.m_GpuHandle)
		, m_NumHandles(Allocation.m_NumHandles)
		, DescriptorSize(Allocation.m_NumHandles)
		, Page(std::move(Allocation.Page))
	{
		Allocation.m_Descriptor.ptr = 0;
    	Allocation.m_NumHandles = 0;
    	Allocation.DescriptorSize = 0;
	}
	
	FDescriptorAllocation& FDescriptorAllocation::operator=(FDescriptorAllocation&& Other) noexcept
	{
		Free();
		
		m_Descriptor = Other.m_Descriptor;
		m_GpuHandle = Other.m_GpuHandle;
  		m_NumHandles = Other.m_NumHandles;
    	DescriptorSize = Other.DescriptorSize;
    	Page = std::move( Other.Page );

    	Other.m_Descriptor.ptr = 0;
    	Other.m_NumHandles = 0;
    	Other.DescriptorSize = 0;

    	return *this;
	}
	
	Ref<FDescriptorAllocatorPage> FDescriptorAllocation::GetDescriptorAllocatorPage() const
	{
		return Page;
	}

	bool FDescriptorAllocation::IsNull()
	{
		return m_Descriptor.ptr == 0;
	}
	
	bool FDescriptorAllocation::IsValid()
	{
		return !IsNull();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorAllocation::GetDescriptorHandle(uint32_t Offset) const
	{
		CORE_ASSERT(Offset < m_NumHandles, "GetDescriptorHandle Offset < NumHandles");
		return { m_Descriptor.ptr + DescriptorSize * Offset };
	}

	D3D12_GPU_DESCRIPTOR_HANDLE FDescriptorAllocation::GetGpuHandle(uint32_t Offset) const
	{
		CORE_ASSERT(Offset < m_NumHandles, "GetGpuHandle Offset < NumHandles");
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_GpuHandle, Offset, DescriptorSize);
	}
	
	uint32_t FDescriptorAllocation::GetNumHandles() const
	{
		return m_NumHandles;	
	}
	
	void FDescriptorAllocation::Free()
	{
		if (!IsNull() && Page)
		{
			Page->Free(std::move(*this));

			m_Descriptor.ptr = 0;
        	m_NumHandles = 0;
        	DescriptorSize = 0;
        	Page.reset();
		}
	}
}
