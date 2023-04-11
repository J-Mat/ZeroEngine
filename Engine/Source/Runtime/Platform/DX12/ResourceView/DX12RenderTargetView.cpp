#include "DX12RenderTargetView.h"
#include "../DX12Device.h"

namespace Zero
{
	FDX12RenderTargetView::FDX12RenderTargetView(Ref<FDX12Resource> Resource, D3D12_RENDER_TARGET_VIEW_DESC* RTVPtr /*= nullptr*/) 
		:FResourceView(EResourceViewType::RTV)
		, m_Resource(Resource)
	{
		auto D3DResourece = m_Resource != nullptr ? m_Resource->GetD3DResource() : nullptr;
		m_Descriptor = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		FDX12Device::Get()->GetDevice()->CreateRenderTargetView(
			m_Resource->GetD3DResource().Get(), 
			RTVPtr,
			m_Descriptor.GetDescriptorHandle()
		);
	}


	FDX12RenderTargetView::FDX12RenderTargetView()
		:FResourceView(EResourceViewType::RTV)
	{

	}

	FDX12RenderTargetView::~FDX12RenderTargetView()
	{
		
	}

	FDX12RenderTargetView::FDX12RenderTargetView(FDX12RenderTargetView&& Other) noexcept
	{
		m_Descriptor = std::move(Other.m_Descriptor);
		m_Resource = Other.m_Resource;
		Other.m_Resource.reset();
	}

	FDX12RenderTargetView& FDX12RenderTargetView::operator=(FDX12RenderTargetView&& Other) noexcept
	{
		m_Descriptor = std::move(Other.m_Descriptor);
		m_Resource = Other.m_Resource;
		Other.m_Resource.reset();
    	return *this;
	}

}