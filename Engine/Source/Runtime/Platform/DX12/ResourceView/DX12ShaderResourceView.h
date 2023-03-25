#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/ResourceView.h"
#include "../MemoryManage/Resource/Resource.h"
#include "../MemoryManage/Descriptor/DescriptorAllocation.h"

namespace Zero
{
	class FDX12ShaderResourceView : public FResourceView
	{
	public:
		FDX12ShaderResourceView(Ref<FDX12Resource> Resource, D3D12_SHADER_RESOURCE_VIEW_DESC* SRV = nullptr);
		~FDX12ShaderResourceView();
		Ref<FDX12Resource> GetResource() const
		{
			return m_Resource;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const
		{
			return m_Descriptor.GetDescriptorHandle();
		}
		virtual void* GetNativeResource() override
		{
			return m_Resource->GetD3DResource().Get();
		}
	private:
		Ref<FDX12Resource> m_Resource;
		FDescriptorAllocation  m_Descriptor;
	};
}
