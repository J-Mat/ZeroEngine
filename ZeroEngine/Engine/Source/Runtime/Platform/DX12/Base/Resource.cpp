#include "Resource.h"
#include "../DX12Device.h"
#include "../MemoryManage/ResourceStateTracker.h"

namespace Zero
{
	IResource::IResource()
	{
	}
	IResource::IResource(const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* ClearValue)
	{
		auto* D3dDevice = FDX12Device::Get()->GetDevice();
		
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

	IResource::IResource( ComPtr<ID3D12Resource> Resource, const D3D12_CLEAR_VALUE* ClearValue )
	: m_D3DResource(Resource)
	{
		if (ClearValue)
		{
			m_D3DClearValue = CreateScope<D3D12_CLEAR_VALUE>(*ClearValue);
		}
		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		CheckFeatureSupport();
	}

	void IResource::SetName(const std::wstring& Name)
	{
		m_ResourceName = Name;
		if (m_D3DResource && !m_ResourceName.empty())
		{
			m_D3DResource->SetName(m_ResourceName.c_str());
		}
	}

	void IResource::SetResource(ComPtr<ID3D12Resource> Resource)
	{
		m_D3DResource = Resource;
		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		CheckFeatureSupport();
	}

	IResource::~IResource()
	{ 
		FDX12Device::Get()->Flush();
		m_D3DResource.Reset();
	}

	void IResource::CheckFeatureSupport()
	{
    	auto Desc              = m_D3DResource->GetDesc();
    	m_FormatSupport.Format = Desc.Format;
    	ThrowIfFailed( FDX12Device::Get()->GetDevice()->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
                                                     sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) );
	}
}