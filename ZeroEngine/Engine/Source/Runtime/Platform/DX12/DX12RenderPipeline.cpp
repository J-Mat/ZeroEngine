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
#include "Core/Framework/Library.h"

namespace Zero
{ 
	FDX12RenderPipeline::FDX12RenderPipeline()
	{	
		
	}
	

	void FDX12RenderPipeline::DrawFrame()
	{
		auto& CommandQueue = FDX12Device::Get()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto CommandList = CommandQueue.GetCommandList();
		FDX12Device::Get()->SetRenderCommandList(CommandList);
		auto SwapChain = FDX12Device::Get()->GetSwapChain();


		FApplication::Get().OnDraw();

		FApplication::Get().PostDraw();

		SwapChain->Present();
	}
	
}
