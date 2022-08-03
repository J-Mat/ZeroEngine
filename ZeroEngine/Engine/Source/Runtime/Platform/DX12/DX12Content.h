#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/GraphicContent.h"
#include "./Common/d3dx12.h"
#include <wrl.h>
#include "Core/Base/PublicSingleton.h"
#include "DX12CommandQueue.h"


namespace Zero
{
	class FDX12Content : public FGraphicContext, public IPublicSingleton<FDX12Content>  
	{
	public:
		virtual void Init();
		
		ID3D12Device* GetDevice() { return D3DDevice.Get(); }
		ID3D12Device* GetCommandQueue() { return CommandQueue->GetCommandList() }
		
	
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
		Ref<DX12CommandQueue> CommandQueue;
	}
}
