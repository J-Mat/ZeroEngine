#include "d3dApp.h"
#include <DirectXColors.h>

using namespace DirectX;

class FHelloD3D12App : public FD3DApp
{
public:
	FHelloD3D12App(HINSTANCE hInstance);
	~FHelloD3D12App();
	
	virtual bool Initialize() override;

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;
};


FHelloD3D12App::FHelloD3D12App(HINSTANCE hInstance)
	: FD3DApp(hInstance)
{

}

FHelloD3D12App::~FHelloD3D12App()
{

}

bool FHelloD3D12App::Initialize()
{
	if (!FD3DApp::Initialize())
	{
		return false;
	}
	return true;
}

void FHelloD3D12App::OnResize()
{
	FD3DApp::OnResize();
}

void FHelloD3D12App::Update(const GameTimer& gt)
{
}

void FHelloD3D12App::Draw(const GameTimer& gt)
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.	
	ThrowIfFailed(
		DirectCmdListAlloctor->Reset()
	);
	
	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(
		CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr)
	);
	
	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		)
	);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.	
	CommandList->RSSetViewports(1, &ScreenViewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
		
	// Clear the back buffer and depth buffer.
	CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::Yellow, 0, nullptr);
	CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	CommandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());
	
	// Indicate a state transition on the resource usage.
	CommandList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			GetCurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);
	
	// Done recording commands.	
	ThrowIfFailed(
		CommandList->Close()
	);

	// Add the command list to the queue for execution.
	ID3D12CommandList* CmdLists[] = { CommandList.Get() };
	CommanQueue->ExecuteCommandLists(_countof(CmdLists), CmdLists);


	// swap the back and front bufferss
	ThrowIfFailed(
		SwapChain->Present(0, 0)
	);
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % SwapChainBufferCount;


	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	try
	{
		FHelloD3D12App App(hInst);
		if (!App.Initialize())
		{
			return  0;
		}
		return App.Run();
	}
	catch (DxException& e)
	{
		MessageBox(0, e.ToString().c_str(), 0, 0);
		return 0;
	}
	return 0;
}
