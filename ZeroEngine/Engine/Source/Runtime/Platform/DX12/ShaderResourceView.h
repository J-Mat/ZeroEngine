#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "Base/Resource.h"f
#include "MemoryManage/DescriptorAllocation.h"

namespace Zero
{
	class FShaderResourceView
	{
	public:
		FShaderResourceView(Ref<FResource> Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* SRV = nullptr);
		~FShaderResourceView();
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
