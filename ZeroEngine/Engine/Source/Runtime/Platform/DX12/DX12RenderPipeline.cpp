#include "DX12RenderPipeline.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12CommandQueue.h"
#include "Platform/DX12/DX12CommandList.h"
#include "Platform/DX12/DX12RenderTarget.h"
#include "Platform/DX12/DX12SwapChain.h"
#include "Platform/DX12/DX12Texture2D.h"
#include <type_traits>
#include <memory>

namespace Zero
{ 
	FDX12RenderPipeline::FDX12RenderPipeline()
	{
	}

	void FDX12RenderPipeline::DrawFrame(Ref<FDX12Device> Device)
	{
		auto& CommandQueue = Device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto CommandList = CommandQueue.GetCommandList();
		
		auto SwapChain = Device->GetSwapChain();
		
		const FDX12RenderTarget& RenderTarget = SwapChain->GetRenderTarget();
		Ref<FDX12Texture2D>  Texture = RenderTarget.GetTexture(EAttachmentIndex::Color0);
		
		ZMath::vec4 Color(0.4f, 0.6f, 0.9f, 1.0f);	
		CommandList->ClearTexture(Texture, Color);

		CommandQueue.ExecuteCommandList(CommandList);
		
		SwapChain->Present();
	}
}
