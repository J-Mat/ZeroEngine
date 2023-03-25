#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/ResourceView.h"
#include "../MemoryManage/Resource/Resource.h"
#include "../MemoryManage/Descriptor/DescriptorAllocation.h"

namespace Zero
{
	class FDX12RenderTargetView : public FResourceView
	{
	public:
		FDX12RenderTargetView();
		FDX12RenderTargetView(Ref<FDX12Resource> Resource, D3D12_RENDER_TARGET_VIEW_DESC* RTV = nullptr);
		~FDX12RenderTargetView();
		Ref<FDX12Resource> GetResource() const
		{
			return m_Resource;
		}
		// Copies are not allowed.
		FDX12RenderTargetView( const FDescriptorAllocation& ) = delete;
		FDX12RenderTargetView& operator=( const FDX12RenderTargetView& ) = delete;

		// Move is allowed.
		FDX12RenderTargetView(FDX12RenderTargetView&& Other) noexcept;
		FDX12RenderTargetView& operator=(FDX12RenderTargetView&& Other) noexcept;

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
