#include "DX12RenderPipeline.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12CommandQueue.h"
#include "Platform/DX12/DX12CommandList.h"
#include "Platform/DX12/DX12RenderTarget.h"
#include "Platform/DX12/DX12SwapChain.h"
#include "Platform/DX12/DX12Texture2D.h"
#include <type_traits>
#include <memory>
#include "Render/RenderLayer/OpaqueLayer.h"
#include "World/World.h"

namespace Zero
{ 
	FDX12RenderPipeline::FDX12RenderPipeline()
	{	
	//	m_RenderLayerStack.PushLayer(new FOpaqueLayer())
		
	}

	void FDX12RenderPipeline::DrawFrame()
	{
		UWorld* World = UWorld::GetCurrentWorld();
		Ref<IDevice>  Device = World->GetDevice();
		FRenderItemPool&  RenderItemsPool = World->GetRenderItemPool();
		
		auto& CommandQueue = Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto CommandList = CommandQueue.GetCommandList();
		
		auto SwapChain = Device->GetSwapChain();
		
		const FDX12RenderTarget& RenderTarget = SwapChain->GetRenderTarget();
		Ref<FTexture2D>  Texture = RenderTarget.GetTexture(EAttachmentIndex::Color0);
		FDX12Texture2D* DX12Texture = static_cast<FDX12Texture2D*>(Texture.get());
		
		ZMath::vec4 Color(0.4f, 1.0f, 0.9f, 1.0f);	
		CommandList->ClearTexture(DX12Texture, Color);

		CommandQueue.ExecuteCommandList(CommandList);
		
		SwapChain->Present();
	}
}
