#include "Adapter.h"

namespace Zero
{ 
	FAdapter::FAdapter(ComPtr<IDXGIAdapter4> DxgiAdapter)
		: m_DxgiAdapter(DxgiAdapter)
		, m_Desc{0}
	{
		if (m_DxgiAdapter)
		{
			ThrowIfFailed(m_DxgiAdapter->GetDesc3(&m_Desc));
		}
	}

	Ref<FAdapter> FAdapter::Create(DXGI_GPU_PREFERENCE GpuPreference, bool bUseWarp)
	{
		Ref<FAdapter> Adapter = nullptr;
		ComPtr<IDXGIFactory6> DxgiFactory6;
		ComPtr<IDXGIAdapter>  DxgiAdapter;
		ComPtr<IDXGIAdapter4> DxgiAdapter4;
	
		UINT CreateFactoryFlags = 0;
#if defined( _DEBUG )
		CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
			
		ThrowIfFailed(::CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&DxgiFactory6)));
		
		if (bUseWarp)
		{
			ThrowIfFailed(DxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&DxgiAdapter)));
			ThrowIfFailed(DxgiAdapter.As(&DxgiAdapter4));
		}
		else
		{
			for (UINT i = 0; DxgiFactory6->EnumAdapterByGpuPreference(i, GpuPreference IID_PPV_ARGS(&DxgiAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				if (SUCCEEDED(D3D12CreateDevice(DxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device),
					nullptr)))
				{
					ThrowIfFailed(DxgiAdapter.As(&DxgiAdapter4));
					break;
				}
			}
		}
		
		if (DxgiAdapter4)
		{ 
			Adapter = CreateRef<FAdapter>(DxgiAdapter4);
		}
		return Adapter;
	}
	FAdapterList FAdapter::GetAdapters(DXGI_GPU_PREFERENCE GpuPreference)
	{
		FAdapterList Adapters;

		ComPtr<IDXGIFactory6> DxgiFactory6;
		ComPtr<IDXGIAdapter>  DxgiAdapter;
		ComPtr<IDXGIAdapter4> DxgiAdapter4;

		UINT CreateFactoryFlags = 0;
#if defined( _DEBUG )
		CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		ThrowIfFailed(::CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&DxgiFactory6)));
		for (UINT i = 0; DxgiFactory6->EnumAdapterByGpuPreference(i, GpuPreference IID_PPV_ARGS(&DxgiAdapter)) !=
			DXGI_ERROR_NOT_FOUND;
			++i)
		{
			if (SUCCEEDED(
				D3D12CreateDevice(DxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				ThrowIfFailed(DxgiAdapter.As(&DxgiAdapter4));
				Ref<FAdapter> adapter = CreateRef<FAdapter>(DxgiAdapter4);
				Adapters.push_back(adapter);
			}
		}
		return Adapters;
	};
	
}
