#include "DescriptorAllocation.h"
#include "../../DX12Device.h"
#include "DescriptorAllocatorPage.h" 


namespace Zero
{
	FDescriptorAllocation::FDescriptorAllocation()
		: m_Descriptor{ 0 }
		, m_NumHandles(0)
		, m_DescriptorSize(0)
		, m_Page(nullptr)
	{
	}

	FDescriptorAllocation::FDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptor, uint32_t InNumHandles, uint32_t InDescriptorSize, Ref<FDescriptorAllocatorPage> InPage)
		: m_Descriptor(InDescriptor)
		, m_NumHandles(InNumHandles)
		, m_DescriptorSize(InDescriptorSize)
		, m_Page(InPage)
	{
	}

	FDescriptorAllocation::~FDescriptorAllocation()
	{
		Free();
	}
	
	FDescriptorAllocation::FDescriptorAllocation(FDescriptorAllocation&& Allocation) noexcept
		: m_Descriptor(Allocation.m_Descriptor)
		, m_NumHandles(Allocation.m_NumHandles)
		, m_DescriptorSize(Allocation.m_DescriptorSize)
		, m_Page(std::move(Allocation.m_Page))
	{
		Allocation.m_Descriptor.ptr = 0;
    	Allocation.m_NumHandles = 0;
    	Allocation.m_DescriptorSize = 0;
	}
	
	FDescriptorAllocation& FDescriptorAllocation::operator=(FDescriptorAllocation&& Other) noexcept
	{
		Free();
		
		m_Descriptor = Other.m_Descriptor;
  		m_NumHandles = Other.m_NumHandles;
    	m_DescriptorSize = Other.m_DescriptorSize;
    	m_Page = std::move( Other.m_Page );

    	Other.m_Descriptor.ptr = 0;
    	Other.m_NumHandles = 0;
    	Other.m_DescriptorSize = 0;

    	return *this;
	}
	
	Ref<FDescriptorAllocatorPage> FDescriptorAllocation::GetDescriptorAllocatorPage() const
	{
		return m_Page;
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
		return { m_Descriptor.ptr + m_DescriptorSize * Offset };
	}
	
	uint32_t FDescriptorAllocation::GetNumHandles() const
	{
		return m_NumHandles;	
	}
	
	void FDescriptorAllocation::Free()
	{
		if (!IsNull() && m_Page)
		{
			m_Page->Free(std::move(*this));

			m_Descriptor.ptr = 0;
        	m_NumHandles = 0;
        	m_DescriptorSize = 0;
        	m_Page.reset();
		}
	}
}
