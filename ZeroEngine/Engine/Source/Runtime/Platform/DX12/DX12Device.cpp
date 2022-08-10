#include "DX12Device.h"



namespace Zero
{
	void FDXDevice::Init()
	{
		EnableDebugLayer();
		CreateDevice();
		GetDescriptorSize();
	}
	
	void FDXDevice::EnableDebugLayer()
	{
		#if defined(DEBUG) || defined(_DEBUG)
		{
			ComPtr<ID3D12Debug> DebugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
			DebugController->EnableDebugLayer();
		}
		#endif	
	}
	
	void FDXDevice::CreateDevice()
	{
		
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

		HRESULT  HardwardResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice));
		if (FAILED(HardwardResult))
		{
			ComPtr<IDXGIAdapter> WarpAdapterPtr;
			ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapterPtr)));

			ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice)));
		}
	}
	
	void FDXDevice::GetDescriptorSize()
	{
		RtvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		DsvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		Cbv_Srv_UavDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	
	void FDXDevice::CreateCommandQueue()
	{
	}
}