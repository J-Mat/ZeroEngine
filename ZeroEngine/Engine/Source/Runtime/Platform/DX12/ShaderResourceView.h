#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Base/Resource.h"
#include "MemoryManage/DescriptorAllocation.h"

namespace Zero
{
	class FShaderResoureceView
	{
	public:
		FShaderResoureceView(Ref<FResource> Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* SRV = nullptr);
		~FShaderResoureceView();
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
		FDescriptorAllocation  m_Descriptor;
	};
}
