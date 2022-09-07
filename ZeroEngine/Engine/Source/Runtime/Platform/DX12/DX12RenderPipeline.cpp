#include "DX12RenderPipeline.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12CommandQueue.h"
#include "Platform/DX12/DX12CommandList.h"
#include "Platform/DX12/DX12RenderTarget.h"
#include "Platform/DX12/DX12SwapChain.h"
#include "Platform/DX12/DX12Texture2D.h"
#include <type_traits>
#include <memory>
#include "World/World.h"
#include "Core/Framework/Application.h"

namespace Zero
{ 
	FDX12RenderPipeline::FDX12RenderPipeline()
	{	
	//	m_RenderLayerStack.PushLayer(new FOpaqueLayer())
		
	}
	

	void FDX12RenderPipeline::DrawFrame()
	{
		auto& CommandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		CommandQueue.GetCommandList();
		UWorld* World = UWorld::GetCurrentWorld();
		Ref<IDevice>  Device = World->GetDevice();
		FRenderItemPool&  RenderItemsPool = World->GetRenderItemPool();
		auto SwapChain = m_Device->GetSwapChain();

		FApplication::Get().OnDraw();
		
		CommandQueue.ExecuteCommandList(m_Device->GetRenderCommandList());
		
		SwapChain->Present();
	}
	void FDX12RenderPipeline::SetDevice(Ref<FDX12Device> Device)
	{
		m_Device = Device;
		auto& CommandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_Device->SetRenderCommandList(CommandQueue.GetCommandList());
	}
	
}
