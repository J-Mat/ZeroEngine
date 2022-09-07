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
		/*
		auto& CommandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_Device->SetRenderCommandList(CommandQueue.GetCommandList());
		UWorld* World = UWorld::GetCurrentWorld();
		Ref<IDevice>  Device = World->GetDevice();
		FRenderItemPool&  RenderItemsPool = World->GetRenderItemPool();
		auto SwapChain = m_Device->GetSwapChain();

		auto RenderTarget = SwapChain->GetRenderTarget();
		Ref<FTexture2D>  Texture = RenderTarget->GetTexture(EAttachmentIndex::Color0);
		FDX12Texture2D* test =   static_cast<FDX12Texture2D*>(Texture.get());
		m_Device->GetRenderCommandList()->ClearTexture(test, ZMath::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		//RenderTarget->Bind();
		//RenderTarget->UnBind();
		//FApplication::Get().OnDraw();
		
		CommandQueue.ExecuteCommandList(m_Device->GetRenderCommandList());
		
		SwapChain->Present();
		*/


		auto& CommandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto CommandList = CommandQueue.GetCommandList();

		auto SwapChain = m_Device->GetSwapChain();

		auto RenderTarget = SwapChain->GetRenderTarget();
		Ref<FTexture2D>  Texture = RenderTarget->GetTexture(EAttachmentIndex::Color0);
		FDX12Texture2D* test = static_cast<FDX12Texture2D*>(Texture.get());

		ZMath::vec4 Color(0.4f, 1.0f, 0.9f, 1.0f);
		CommandList->ClearTexture(test, Color);

		CommandQueue.ExecuteCommandList(CommandList);

		SwapChain->Present();
	}
	void FDX12RenderPipeline::SetDevice(Ref<FDX12Device> Device)
	{
		m_Device = Device;
		auto& CommandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	
}
