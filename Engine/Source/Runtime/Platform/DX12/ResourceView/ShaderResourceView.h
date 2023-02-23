#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "../MemoryManage/Resource/Resource.h"
#include "../MemoryManage/Descriptor/DescriptorAllocation.h"

namespace Zero
{
	class FShaderResourceView
	{
	public:
		FShaderResourceView(Ref<FDX12Resource> Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* SRV = nullptr);
		~FShaderResourceView();
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
		FDescriptorAllocation  m_Descriptor;
	};
}
