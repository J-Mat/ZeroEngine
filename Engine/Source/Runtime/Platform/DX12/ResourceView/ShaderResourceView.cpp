#include "ShaderResourceView.h"
#include "../DX12Device.h"

namespace Zero
{
	FShaderResourceView::FShaderResourceView(Ref<FDX12Resource> Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* SRVPtr)
		: m_Resource(Resource)
	{
		auto D3DResourece = m_Resource != nullptr ? m_Resource->GetD3DResource() : nullptr;
		m_Descriptor = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		FDX12Device::Get()->GetDevice()->CreateShaderResourceView(
			m_Resource->GetD3DResource().Get(), 
			SRVPtr,
			m_Descriptor.GetDescriptorHandle()
		);
	}
	FShaderResourceView::~FShaderResourceView()
	{
	}
}