#include "DX12DepthStencilView.h"
#include "../DX12Device.h"

namespace Zero
{
	FDX12DepthStencilView::FDX12DepthStencilView(Ref<FDX12Resource> Resource, D3D12_DEPTH_STENCIL_VIEW_DESC* DSVPtr /*= nullptr*/) : 
		FResourceView(EResourceViewType::DSV)
		, m_Resource(Resource)
	{
		auto D3DResourece = m_Resource != nullptr ? m_Resource->GetD3DResource() : nullptr;
		m_Descriptor = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		FDX12Device::Get()->GetDevice()->CreateDepthStencilView(
			m_Resource->GetD3DResource().Get(), 
			DSVPtr,
			m_Descriptor.GetDescriptorHandle()
		);
	}

	FDX12DepthStencilView::~FDX12DepthStencilView()
	{
	}
}