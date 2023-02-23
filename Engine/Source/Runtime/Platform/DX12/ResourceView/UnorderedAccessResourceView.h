#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "../MemoryManage/Resource/Resource.h"
#include "../MemoryManage/Descriptor/DescriptorAllocation.h"

namespace Zero
{
	class FUnorderedAccessResourceView
	{
	public:
		FUnorderedAccessResourceView(
			Ref<FDX12Resource> Resource, 
			Ref<FDX12Resource> CounterResource = nullptr,
			const D3D12_UNORDERED_ACCESS_VIEW_DESC* UAVPtr = nullptr);
		~FUnorderedAccessResourceView();
		Ref<FDX12Resource> GetResource() const
		{
			return m_Resource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const
		{
			return m_Descriptor.GetDescriptorHandle();
		}
	private:
		Ref<FDX12Resource> m_Resource;
		Ref<FDX12Resource> m_CounterResource;
		FDescriptorAllocation  m_Descriptor;
			
	};
}