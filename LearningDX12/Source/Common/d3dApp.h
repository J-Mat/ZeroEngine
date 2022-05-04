#pragma  once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


#include "d3dUtil.h"
#include "GameTimer.h"

using namespace Microsoft::WRL;

class FD3DApp
{
protected: 
	FD3DApp(HINSTANCE HInstance);
	virtual ~FD3DApp();
	GameTimer Timer;
	HINSTANCE AppInstance = nullptr; // application instance handle
	HWND MainWnd = nullptr;
	bool      bAppPaused = false;  // is the application paused?
	bool      bMinimized = false;  // is the application minimized?
	bool      bMaximized = false;  // is the application maximized?
	bool      bResizing = false;   // are the resize bars being dragged?
	bool      bFullscreenState = false;// fullscreen enabled
	
	bool X4MSAAState = false;
	UINT X4MSAAQuality = 0;
	
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGISwapChain> SwapChain;
	ComPtr<ID3D12Device> D3DDevice;
	ComPtr<ID3D12Fence> Fence;
	UINT64 CurrentFence = 0;
	
	ComPtr<ID3D12CommandQueue> CommanQueue;
	ComPtr<ID3D12CommandAllocator> DirectCmdListAlloctor;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	static const int SwapChainBufferCount = 2;
	int CurrentBuffer = 0;
	ComPtr<ID3D12Resource> SwapChangeBuffers[SwapChainBufferCount];
	ComPtr<ID3D12Resource> DepthStencilBuffer;
	
	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12DescriptorHeap> DsvHeap;

	D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;
	
	UINT RtvDescriptorSize = 0;
	UINT DsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
	
	std::wstring MainWndCaption = L"d3d App";
	D3D_DRIVER_TYPE D3DDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	int ClientWidth = 800;
	int ClientHeight = 600;
	
protected:
	static FD3DApp* App;
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool InitWindow();
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }
public:
	static FD3DApp* GetApp() { return App; };
};


