#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "../MemoryManage/Resource/Resource.h"
#include "../MemoryManage/Descriptor/DescriptorAllocation.h"
#include "Render/RHI/ResourceView.h"

namespace Zero
{
	class FDX12UnorderedAccessResourceView : public FResourceView
	{
	public:
		FDX12UnorderedAccessResourceView(
			Ref<FDX12Resource> Resource, 
			Ref<FDX12Resource> CounterResource = nullptr,
			const D3D12_UNORDERED_ACCESS_VIEW_DESC* UAVPtr = nullptr);
		~FDX12UnorderedAccessResourceView();
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
		Ref<FDX12Resource> m_CounterResource;
		FDescriptorAllocation  m_Descriptor;
			
	};
}