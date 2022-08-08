#include "Resource.h"

namespace Zero
{
	FResource::FResource(FDX12Device &InDevice, const D3D12_RESOURCE_DESC& ResourceDesc,
				const D3D12_CLEAR_VALUE* ClearValue = nullptr)
	: Device(InDevice)
	{
		auto D3dDevice = InDevice.GetDevice();
		
		if (ClearValue)
		{
			D3DClearValue = CreateScope<D3D12_CLEAR_VALUE>(*ClearValue);
		}
		ThrowIfFailed(
			D3dDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &ResourceDesc,
				D3D12_RESOURCE_STATE_COMMON, D3DClearValue.get(), IID_PPV_ARGS( &D3DResource )));
			)
		)

		// to-do resource track
		
		CheckFeatureSupport();
	}

	FResource::FResource( FDX12Device& InDevice, ComPtr<ID3D12Resource> Resource,
                    const D3D12_CLEAR_VALUE* ClearValue )
	: Device(InDevice)
	, D3Desource(Resource)
	{
		if (ClearValue)
		{
			D3DClearValue = CreateScope<D3D12_CLEAR_VALUE>(*ClearValue);
		}
		CheckFeatureSupport();
	}

	void FResource::CheckFeatureSupport()
	{
    	auto Desc              = D3Desource->GetDesc();
    	FormatSupport.Format = Desc.Format;
    	ThrowIfFailed( Device.GetDevice()->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &FormatSupport,
                                                     sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) );
	}
}