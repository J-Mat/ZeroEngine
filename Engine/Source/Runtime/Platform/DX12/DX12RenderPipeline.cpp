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
		static auto* Device = FDX12Device::Get();
		Device->BeginFrame();

		FCommandListHandle InitWorldCommandListHandle = Device->GenerateCommanList(ERenderPassType::Graphics);
		Device->SetInitWorldCommandList(InitWorldCommandListHandle);
		
		FCommandListHandle MipComandListHandle = Device->GenerateCommanList(ERenderPassType::Compute);
		Device->SetMipCommandList(MipComandListHandle);

		auto SwapChain = Device->GetSwapChain();


		FApplication::Get().OnDraw();

		FApplication::Get().PostDraw();

		auto InitComandList = Device->GetCommandList(InitWorldCommandListHandle);
		Device->GetCommandQueue(ERenderPassType::Graphics).ExecuteCommandList(InitComandList);
		auto MipComandList = Device->GetMipCommandList();
		Device->GetCommandQueue(ERenderPassType::Compute).ExecuteCommandList(MipComandList);
		
		Device->ExecuteSingleThreadCommandLists();


		SwapChain->Present();

		Device->EndFrame();
	}
}
