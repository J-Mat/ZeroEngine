#include "LinearDynamicAllocator.h"

namespace Zero
{
	FLinearDynamicAllocator::FLinearDynamicAllocator(size_t MaxSizeInBytes)
		:m_LinnerAllocator(MaxSizeInBytes)
	{
		FBufferDesc Desc{
			.Size = uint32_t(MaxSizeInBytes),
			.ResourceUsage = EResourceUsage::Upload,
			.ResourceBindFlag = EResourceBindFlag::ShaderResource,
		};
		m_Buffer = CreateScope<FDX12Buffer>(Desc);
		m_CpuAddress = m_Buffer->Map();
	}

	FDynamicAllocation FLinearDynamicAllocator::Allocate(size_t SizeInBytes, size_t Alignment /*= 0*/)
	{
		size_t Offset = INVALID_OFFSET;
		{
			std::lock_guard<std::mutex> Guard(m_AllocMutex);
			Offset = m_LinnerAllocator.Allocate(SizeInBytes, Alignment);
		}

		if (Offset != INVALID_OFFSET)
		{
			FDynamicAllocation Allocation{};
			Allocation.CpuAddress = reinterpret_cast<uint8_t*>(m_CpuAddress) + Offset;
			Allocation.GpuAddress = m_Buffer->GetGPUAddress() + Offset;
			Allocation.UploadBuffer = m_Buffer->GetResource();
			Allocation.Offset = Offset;
			Allocation.Size = SizeInBytes;

			return Allocation;
		}
		else
		{
			CORE_ASSERT(false, "Not enough memory in Dynamic Allocator. Increase the size to avoid this error!");
			return FDynamicAllocation{}; //return optional?
		}
			
	}

	void FLinearDynamicAllocator::CleanUpAllocations()
	{
		std::lock_guard<std::mutex> guard(m_AllocMutex);
		m_LinnerAllocator.Clear();
	}

}
