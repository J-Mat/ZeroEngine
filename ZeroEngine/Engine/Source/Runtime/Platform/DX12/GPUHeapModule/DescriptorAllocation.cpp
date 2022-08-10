#include "DescriptorAllocation.h"


namespace Zero
{
	FDescriptorAllocation::FDescriptorAllocation()
		: Descriptor{ 0 }
		, NumHandles(0)
		, DescriptorSize(0)
		, Page(nullptr)
	{
	}

	FDescriptorAllocation::FDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptor, uint32_t InNumHandles, uint32_t InDescriptorSize, Ref<FDescriptorAllocatorPage> InPage)
		: Descriptor(InDescriptor)
		, NumHandles(InNumHandles)
		, DescriptorSize(InDescriptorSize)
		, Page(InPage)
	{
	}

	FDescriptorAllocation::~FDescriptorAllocation()
	{
		Free();
	}
	
	FDescriptorAllocation::FDescriptorAllocation(FDescriptorAllocation&& Allocation) noexcept
		: Descriptor(Allocation.Descriptor)
		, NumHandles(Allocation.NumHandles)
		, DescriptorSize(Allocation.NumHandles)
		, Page(std::move(Allocation.Page))
	{
		Allocation.Descriptor.ptr = 0;
    	Allocation.NumHandles = 0;
    	Allocation.DescriptorSize = 0;
	}
	
	FDescriptorAllocation& FDescriptorAllocation::operator=(FDescriptorAllocation&& Other) noexcept
	{
		Free();
		
		Descriptor = Other.Descriptor;
  		NumHandles = Other.NumHandles;
    	DescriptorSize = Other.DescriptorSize;
    	Page = std::move( Other.Page );

    	Other.Descriptor.ptr = 0;
    	Other.NumHandles = 0;
    	Other.DescriptorSize = 0;

    	return *this;
	}
	
	Ref<FDescriptorAllocatorPage> FDescriptorAllocation::GetDescriptorAllocatorPage() const
	{
		return Page;
	}

	bool FDescriptorAllocation::IsNull()
	{
		return Descriptor.ptr == 0;
	}
	
	bool FDescriptorAllocation::IsValid()
	{
		return !IsNull();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FDescriptorAllocation::GetDescriptorHandle(uint32_t Offset) const
	{
		CORE_ASSERT(Offset < NumHandles, "GetDescriptorHandle Offset < NumHandles");
		return { Descriptor.ptr + DescriptorSize * Offset };
	}
	
	uint32_t FDescriptorAllocation::GetNumHandles() const
	{
		return NumHandles;	
	}
	
	void FDescriptorAllocation::Free()
	{
		if (!IsNull() && Page)
		{
			Page->Free(std::move(*this));

			Descriptor.ptr = 0;
        	NumHandles = 0;
        	DescriptorSize = 0;
        	Page.reset();
		}
	}
}
