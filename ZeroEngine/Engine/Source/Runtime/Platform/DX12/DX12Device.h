#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/GraphicDevice.h"
#include "./Common/d3dx12.h"
#include <wrl.h>
#include "DX12CommandQueue.h"


namespace Zero
{
	class FDX12Device : public IDevice
	{
	public:
		virtual void Init();

		ID3D12Device* GetDevice() { return D3DDevice.Get(); }


	private:
		void EnableDebugLayer();
		void CreateDevice();
		void GetDescriptorSize();
		void CreateCommandQueue();

	private:
		UINT RtvDescriptorSize;
		UINT DsvDescriptorSize;
		UINT Cbv_Srv_UavDescriptorSize;
		ComPtr<IDXGIFactory4> DxgiFactory;
		ComPtr<ID3D12Device> D3DDevice;

		Scope<FDX12CommandQueue> DirectCommandQueue;
		Scope<FDX12CommandQueue> ComputeCommandQueue;
		Scope<FDX12CommandQueue> CopyCommandQueue;
	};
}
