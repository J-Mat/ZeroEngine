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
		auto CommandList = CommandQueue.GetCommandList();
		m_Device->SetRenderCommandList(CommandList);
		auto SwapChain = m_Device->GetSwapChain();

		
		/*

		auto RenderTarget = m_Device->GetSwapChain()->GetRenderTarget();
		RenderTarget->Bind();
		RenderTarget->ClearBuffer();
		Ref<FTexture2D> Texuture =  RenderTarget->GetTexture(EAttachmentIndex::Color0);
		RenderTarget->UnBind();
		*/

		FApplication::Get().OnDraw();

		SwapChain->Present();
	}
	void FDX12RenderPipeline::SetDevice(Ref<IDevice> Device)
	{
		m_Device = static_cast<FDX12Device*>(Device.get())->AsShared();
		auto& CommandQueue = m_Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	}
	
}
