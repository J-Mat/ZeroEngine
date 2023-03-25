#include "DX12ShaderResourceView.h"
#include "../DX12Device.h"

namespace Zero
{
	FDX12ShaderResourceView::FDX12ShaderResourceView(Ref<FDX12Resource> Resource, D3D12_SHADER_RESOURCE_VIEW_DESC* SRV /*= nullptr*/) : FResourceView(EResourceViewType::SRV) 
		,m_Resource(Resource)
	{
		auto D3DResourece = m_Resource != nullptr ? m_Resource->GetD3DResource() : nullptr;
		m_Descriptor = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		FDX12Device::Get()->GetDevice()->CreateShaderResourceView(
			m_Resource->GetD3DResource().Get(),
			SRV,
			m_Descriptor.GetDescriptorHandle()
		);
	}
	FDX12ShaderResourceView::~FDX12ShaderResourceView()
	{
	}
}