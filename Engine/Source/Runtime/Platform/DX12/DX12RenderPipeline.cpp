#include "DX12RenderPipeline.h"
#include "Platform/DX12/DX12Device.h"
#include "Platform/DX12/DX12CommandQueue.h"
#include "Platform/DX12/DX12CommandList.h"
#include "Platform/DX12/DX12RenderTarget2D.h"
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
		FDX12Device::Get()->BeginFrame();
		FCommandListHandle SingleThreadCommandListHandle = FDX12Device::Get()->GenerateCommanList(ERenderPassType::Graphics);
		FDX12Device::Get()->SetSingleThreadCommandList(SingleThreadCommandListHandle);

		FCommandListHandle InitWorldCommandListHandle = FDX12Device::Get()->GenerateCommanList(ERenderPassType::Graphics);
		FDX12Device::Get()->SetInitWorldCommandList(InitWorldCommandListHandle);
		
		FCommandListHandle MipComandListHandle = FDX12Device::Get()->GenerateCommanList(ERenderPassType::Compute);
		FDX12Device::Get()->SetMipCommandList(MipComandListHandle);

		auto SwapChain = FDX12Device::Get()->GetSwapChain();


		FApplication::Get().OnDraw();

		FApplication::Get().PostDraw();

		auto InitComandList = FDX12Device::Get()->GetCommandList(InitWorldCommandListHandle);
		FDX12Device::Get()->GetCommandQueue(ERenderPassType::Graphics).ExecuteCommandList(InitComandList);
		auto MipComandList = FDX12Device::Get()->GetMipCommandList();
		FDX12Device::Get()->GetCommandQueue(ERenderPassType::Compute).ExecuteCommandList(MipComandList);
		auto SingleThreadComandList = FDX12Device::Get()->GetCommandList(SingleThreadCommandListHandle);

		FDX12Device::Get()->GetCommandQueue(ERenderPassType::Graphics).ExecuteCommandList_Raw(SingleThreadComandList);

		SwapChain->Present();

		FDX12Device::Get()->EndFrame();
	}
}
