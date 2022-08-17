#include "Resource.h"
#include "../GPUMemory/ResourceStateTracker.h"

namespace Zero
{
	IResource::IResource(FDX12Device& InDevice)
	: m_Device(InDevice)
	{
	}
	IResource::IResource(FDX12Device &InDevice, const D3D12_RESOURCE_DESC& ResourceDesc,
				const D3D12_CLEAR_VALUE* ClearValue = nullptr)
	: m_Device(InDevice)
	{
		auto D3dDevice = InDevice.GetDevice();
		
		if (ClearValue)
		{
			m_D3DClearValue = CreateScope<D3D12_CLEAR_VALUE>(*ClearValue);
		}
		ThrowIfFailed(
			D3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &ResourceDesc,
				D3D12_RESOURCE_STATE_COMMON, m_D3DClearValue.get(), IID_PPV_ARGS( &m_D3DResource )));

		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		
		CheckFeatureSupport();
	}

	IResource::IResource( FDX12Device& InDevice, ComPtr<ID3D12Resource> Resource,
                    const D3D12_CLEAR_VALUE* ClearValue )
	: m_Device(InDevice)
	, m_D3DResource(Resource)
	{
		if (ClearValue)
		{
			m_D3DClearValue = CreateScope<D3D12_CLEAR_VALUE>(*ClearValue);
		}
		CheckFeatureSupport();
	}

	void IResource::SetResource(ComPtr<ID3D12Resource> Resource)
	{
		m_D3DResource = Resource;
		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		CheckFeatureSupport();
	}

	void IResource::CheckFeatureSupport()
	{
    	auto Desc              = m_D3DResource->GetDesc();
    	m_FormatSupport.Format = Desc.Format;
    	ThrowIfFailed( m_Device.GetDevice()->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
                                                     sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) );
	}
}