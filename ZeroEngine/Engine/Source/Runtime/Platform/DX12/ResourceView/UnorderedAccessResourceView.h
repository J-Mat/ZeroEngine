#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "../Resource.h"
#include "../MemoryManage/DescriptorAllocation.h"

namespace Zero
{
	class FUnorderedAccessResourceView
	{
	public:
		FUnorderedAccessResourceView(
			Ref<FResource> Resource, 
			Ref<FResource> CounterResource = nullptr,
			const D3D12_UNORDERED_ACCESS_VIEW_DESC* UAVPtr = nullptr);
		~FUnorderedAccessResourceView();
		Ref<FResource> GetResource() const
		{
			return m_Resource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const
		{
			return m_Descriptor.GetDescriptorHandle();
		}
	private:
		Ref<FResource> m_Resource;
		Ref<FResource> m_CounterResource;
		FDescriptorAllocation  m_Descriptor;
			
	};
}