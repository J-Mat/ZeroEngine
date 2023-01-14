#include "Resource.h"
#include "../DX12Device.h"
#include "../MemoryManage/ResourceStateTracker.h"
#include "Utils.h"

namespace Zero
{
	FResource::FResource()
	{
	}
	FResource::FResource(const std::string& ResourceName, const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* ClearValue)
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

		m_D3DResource->SetName(Utils::StringToLPCWSTR(ResourceName));

		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
	
		CheckFeatureSupport();
	}

	FResource::FResource( ComPtr<ID3D12Resource> Resource, const D3D12_CLEAR_VALUE* ClearValue )
	: m_D3DResource(Resource)
	{
		if (ClearValue)
		{
			m_D3DClearValue = CreateScope<D3D12_CLEAR_VALUE>(*ClearValue);
		}
		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		CheckFeatureSupport();
	}

	void FResource::SetName(const std::wstring& Name)
	{
		m_ResourceName = Name;
		if (m_D3DResource && !m_ResourceName.empty())
		{
			m_D3DResource->SetName(m_ResourceName.c_str());
		}
	}

	void FResource::SetResource(ComPtr<ID3D12Resource> Resource)
	{
		m_D3DResource = Resource;
		FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
		CheckFeatureSupport();
	}

	FResource::~FResource()
	{ 
		FDX12Device::Get()->Flush();
		m_D3DResource.Reset();
	}

	void FResource::CheckFeatureSupport()
	{
    	auto Desc              = m_D3DResource->GetDesc();
    	m_FormatSupport.Format = Desc.Format;
    	ThrowIfFailed( FDX12Device::Get()->GetDevice()->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
                                                     sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) );
	}
}