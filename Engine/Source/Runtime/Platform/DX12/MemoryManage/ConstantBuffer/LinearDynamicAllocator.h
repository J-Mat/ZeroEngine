#pragma once
#include "Core.h"
#include "../../Common/DX12Header.h"
#include "DynamicAllocation.h"
#include "LinearAllocator.h"
#include "../../DX12Buffer.h"
#include <mutex>


namespace Zero
{
	class FDX12Buffer;
	class FLinearDynamicAllocator
	{
	public:
		FLinearDynamicAllocator(size_t MaxSizeInBytes);
		~FLinearDynamicAllocator() = default;
		FDynamicAllocation Allocate(size_t SizeInBytes, size_t Alignment = 0);
		void CleanUpAllocations();
	private:
		FLinearAllocator m_LinnerAllocator;
		std::mutex m_AllocMutex;
		std::unique_ptr<FDX12Buffer> m_Buffer;
		void* m_CpuAddress;
	};
}