#include "UnorderedAccessResourceView.h"
#include "../DX12Device.h"

namespace Zero
{
	FUnorderedAccessResourceView::FUnorderedAccessResourceView(Ref<FResource> Resource, Ref<FResource> CounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* UAVPtr)
		: m_Resource(Resource)
		, m_CounterResource(CounterResource)
	{

		auto D3DResource = m_Resource ? m_Resource->GetD3DResource() : nullptr;
		auto D3DCounterResource = m_CounterResource ? m_CounterResource->GetD3DResource() : nullptr;

		if (m_Resource)
		{
			auto ResourceDesc = m_Resource->GetD3D12ResourceDesc();

			CORE_ASSERT((ResourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0, "Can not support unordered access!");
		}

		m_Descriptor = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		FDX12Device::Get()->GetDevice()->CreateUnorderedAccessView(
			D3DResource.Get(),
			D3DCounterResource.Get(),
			UAVPtr,
			m_Descriptor.GetDescriptorHandle()
		);
		
	}

	FUnorderedAccessResourceView::~FUnorderedAccessResourceView()
	{
	}
}

