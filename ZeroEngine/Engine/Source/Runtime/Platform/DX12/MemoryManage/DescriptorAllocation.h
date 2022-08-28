#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"

namespace Zero
{
	class FDX12Device;
	class FDescriptorAllocatorPage;
	class FDescriptorAllocation
	{
	public:
		FDescriptorAllocation();

		FDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptor, uint32_t InNumHandles, uint32_t InDescriptorSize,
			Ref<FDescriptorAllocatorPage> InPage);

		~FDescriptorAllocation();

		// Copies are not allowed.
    	FDescriptorAllocation( const FDescriptorAllocation& ) = delete;
    	FDescriptorAllocation& operator=( const FDescriptorAllocation& ) = delete;

		// Move is allowed.
    	FDescriptorAllocation( FDescriptorAllocation&& Allocation ) noexcept;
    	FDescriptorAllocation& operator=( FDescriptorAllocation&& Other ) noexcept;


		// Get the heap that this allocation came from.
		// (For internal use only).
    	Ref<FDescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

		bool IsNull();
		bool IsValid();
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t Offset = 0) const;
		uint32_t GetNumHandles() const;
	private:
		void Free();

		D3D12_CPU_DESCRIPTOR_HANDLE m_Descriptor;
		uint32_t m_NumHandles;
		uint32_t DescriptorSize;
		
		Ref<FDescriptorAllocatorPage> Page;
	};
}

