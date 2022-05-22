#include "d3dApp.h"
#include <WindowsX.h>
#include <Windows.h>

FD3DApp* FD3DApp::App = nullptr;



LRESULT FD3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			bAppPaused = true;
			Timer.Stop();
		}
		else
		{
			bAppPaused = false;
			Timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		ClientWidth = LOWORD(lParam);
		ClientHeight = HIWORD(lParam);
		if (D3DDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				bAppPaused = true;
				bMinimized = true;
				bMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				bAppPaused = false;
				bMinimized = false;
				bMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (bMinimized)
				{
					bAppPaused = false;
					bMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (bMaximized)
				{
					bAppPaused = false;
					bMaximized = false;
					OnResize();
				}
				else if (bResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		bAppPaused = true;
		bResizing = true;
		Timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		bAppPaused = false;
		bResizing = false;
		Timer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2)
			Set4xMsaaState(!X4MSAAState);

		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


void FD3DApp::LogOutputDisplayModes(IDXGIOutput* Output, DXGI_FORMAT Format)
{
	UINT Count = 0;
	UINT Flags = 0;

	// Call with nullptr to get list count.	
	Output->GetDisplayModeList(Format, Flags, &Count, nullptr);
	
	std::vector<DXGI_MODE_DESC> ModelList(Count);
	Output->GetDisplayModeList(Format, Flags, &Count, &ModelList[0]);
	
	for (auto& Model : ModelList)
	{
		UINT N = Model.RefreshRate.Numerator;
		UINT D = Model.RefreshRate.Denominator;
		std::wstring Text =
		 	L"Width = " + std::to_wstring(Model.Width) + L" " +
            L"Height = " + std::to_wstring(Model.Height) + L" " +
            L"Refresh = " + std::to_wstring(N) + L"/" + std::to_wstring(D) +
            L"\n";
		::OutputDebugString(Text.c_str());
	}

}

void FD3DApp::LogAdapterOutputs(IDXGIAdapter* InAdapterPtr)
{
	UINT Index = 0;
	IDXGIOutput* Output = nullptr;
	while (InAdapterPtr->EnumOutputs(Index, &Output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC Desc;
		Output->GetDesc(&Desc);

		std::wstring Text = L"***Output: ";
		Text += Desc.DeviceName;
		Text += L"\n";
		::OutputDebugString(Text.c_str());
		
		ReleaseCom(Output);
	
		++Index;
	}
}

void FD3DApp::LogAdapters()
{
	UINT Index = 0;
	IDXGIAdapter* AdapterPtr = nullptr;
	std::vector<IDXGIAdapter*> AdapterList;	
	while (DxgiFactory->EnumAdapters(Index, &AdapterPtr) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC Desc;
		AdapterPtr->GetDesc(&Desc);
		
		std::wstring Text = L"***Adapter: ";
		Text += Desc.Description;
		Text += L"\n";
		
		OutputDebugString(Text.c_str());
		
		AdapterList.push_back(AdapterPtr);
		
		++Index;
	}
	
	for (int i = 0; i < AdapterList.size(); ++i)
	{
		LogAdapterOutputs(AdapterList[i]);
		ReleaseCom(AdapterList[i]);
	}
	
}

void FD3DApp::CreateCommandObjects()
{	
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(D3DDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
	
	ThrowIfFailed(D3DDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(DirectCmdListAlloctor.GetAddressOf()))
	);
	
	ThrowIfFailed(
		D3DDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			DirectCmdListAlloctor.Get(), // Associated command allocator
			nullptr,                     // Initial PipelineStateObject
			IID_PPV_ARGS(CommandList.GetAddressOf())
		)
	);

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	CommandList->Close();
}

void FD3DApp::CreateSwapChain()
{  
	// Release the previous swapchain we will be recreating.	
	SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = ClientWidth;
	SwapChainDesc.BufferDesc.Height = ClientHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = BackBufferFormat;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChainDesc.SampleDesc.Count = X4MSAAState ? 4 : 1;
	SwapChainDesc.SampleDesc.Quality = X4MSAAState ? (X4MSAAQuality - 1) : 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = SwapChainBufferCount;
	SwapChainDesc.OutputWindow = MainWnd;
	SwapChainDesc.Windowed = true;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(
		DxgiFactory->CreateSwapChain(
			CommandQueue.Get(),
			&SwapChainDesc,
			SwapChain.GetAddressOf()
		)
	);
}

void FD3DApp::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc;
	RtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RtvHeapDesc.NodeMask = 0; 
	ThrowIfFailed(
		D3DDevice->CreateDescriptorHeap(
			&RtvHeapDesc, 
			IID_PPV_ARGS(RTVHeap.GetAddressOf()))
	);
	
	
	D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc;
	DsvHeapDesc.NumDescriptors = 1;
	DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	
	DsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		D3DDevice->CreateDescriptorHeap(
			&DsvHeapDesc, 
			IID_PPV_ARGS(DSVHeap.GetAddressOf()))
	);
}

void FD3DApp::FlushCommandQueue()
{
	// Advance the fence value to mark commands up to this fence point.
	++CurrentFence;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(
		CommandQueue->Signal(Fence.Get(), CurrentFence)
	);
	
	if (Fence->GetCompletedValue() < CurrentFence)
	{
		HANDLE EventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(
			Fence->SetEventOnCompletion(CurrentFence, EventHandle)
		); 

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(EventHandle, INFINITE);
		CloseHandle(EventHandle);
	}
}

ID3D12Resource* FD3DApp::GetCurrentBackBuffer() const
{
	return SwapChangeBuffers[CurrentBackBufferIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3DApp::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RTVHeap->GetCPUDescriptorHandleForHeapStart(),
		CurrentBackBufferIndex,
		RtvDescriptorSize
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE FD3DApp::GetDepthStencilView() const
{
	return DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

void FD3DApp::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int FrameCnt = 0;
	static float TimeElapsed = 0.0f;

	FrameCnt++;

	// Compute averages over one second period.
	if ((Timer.TotalTime() - TimeElapsed) >= 1.0f)
	{
		float fps = (float)FrameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = MainWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(MainWnd, windowText.c_str());

		// Reset for next average.
		FrameCnt = 0;
		TimeElapsed += 1.0f;
	}
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return FD3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}


FD3DApp::FD3DApp(HINSTANCE hInstance)
	: hAppInstance(hInstance)
{
	FD3DApp::App = this;
}

FD3DApp::~FD3DApp()
{

}


bool FD3DApp::InitMainWindow()
{
	wchar_t szAppclassName[] = L"FirstWin32";
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"aa";


	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}
	
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	//AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	
	int width = R.right - R.left;
	int height = R.bottom - R.top;
	MainWnd = CreateWindow(L"aa",
		L"d3d App",
		WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU, 
		200, 200, width, height,
		NULL,
		NULL,
		hAppInstance, 
		0);


	if (!MainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}
	
	ShowWindow(MainWnd, SW_SHOW);
	UpdateWindow(MainWnd);
	
	return true;
}

bool FD3DApp::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> DebugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
		DebugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

	HRESULT  HardwardResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice));
	if (FAILED(HardwardResult))
	{
		ComPtr<IDXGIAdapter> WarpAdapterPtr;
		ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapterPtr)));

		ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice)));
	}

	ThrowIfFailed(D3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	RtvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	CbvSrvUavDescriptorSize = D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS QualityLevels;
	QualityLevels.Format = BackBufferFormat;
	QualityLevels.SampleCount = 4;
	QualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	QualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(D3DDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&QualityLevels,
		sizeof(QualityLevels)
	));
	
	X4MSAAQuality = QualityLevels.NumQualityLevels;
	
	assert(X4MSAAQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif // _DEBUG

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void FD3DApp::OnResize()
{
	assert(D3DDevice);
	assert(SwapChain);
	assert(DirectCmdListAlloctor);

	FlushCommandQueue();

	ThrowIfFailed(
		CommandList->Reset(DirectCmdListAlloctor.Get(), nullptr)
	);
	
	// Release the previous resources we will be recreating.
	for (int Index = 0; Index < SwapChainBufferCount; ++Index)
	{
		SwapChangeBuffers[Index].Reset();
	}

	DepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(
		SwapChain->ResizeBuffers(
			SwapChainBufferCount,
			ClientWidth, ClientHeight,
			BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		)
	);	
	CurrentBackBufferIndex = 0;
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE RTVHeapHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		ThrowIfFailed(
			SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChangeBuffers[i]))
		);	
		D3DDevice->CreateRenderTargetView(SwapChangeBuffers[i].Get(), nullptr, RTVHeapHandle);
		RTVHeapHandle.Offset(1, RtvDescriptorSize);
	}
	
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC DepthStencilDesc;
	DepthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DepthStencilDesc.Alignment = 0;
	DepthStencilDesc.Width = ClientWidth;
	DepthStencilDesc.Height = ClientHeight;
	DepthStencilDesc.DepthOrArraySize = 1;
	DepthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format. 
	DepthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DepthStencilDesc.SampleDesc.Count = X4MSAAState ? 4 : 1;
	DepthStencilDesc.SampleDesc.Quality = X4MSAAState ? (X4MSAAQuality - 1) : 0;
	DepthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	D3D12_CLEAR_VALUE OptClear;
	OptClear.Format = DepthStencilFormat;
	OptClear.DepthStencil.Depth = 1.0f;
	OptClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&DepthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&OptClear,
			IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf())
		)
	);	

	// Create descriptor to mip level 0 of entire resource using the format of the resource.	
	D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
	DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Format = DepthStencilFormat;
	DSVDesc.Texture2D.MipSlice = 0;
	D3DDevice->CreateDepthStencilView(DepthStencilBuffer.Get(), &DSVDesc, GetDepthStencilView());
	
	// Transition the resource from its initial state to be used as a depth buffer.
	CommandList->ResourceBarrier(
		1, 
		&CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE
		)
	);
	
	// Execute the resize commands.
	ThrowIfFailed(
		CommandList->Close()
	);
	ID3D12CommandList* CmdLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdLists), CmdLists);

	// Wait until resize is complete.
	FlushCommandQueue();


	// Update the viewport transform to cover the client area.
	ScreenViewport.TopLeftX = 0;
	ScreenViewport.TopLeftY = 0;
	ScreenViewport.Width = static_cast<float>(ClientWidth);
	ScreenViewport.Height = static_cast<float>(ClientHeight);
	ScreenViewport.MinDepth = 0.0f;
	ScreenViewport.MaxDepth = 1.0f;

	ScissorRect = { 0, 0, ClientWidth, ClientHeight };
}

float FD3DApp::AspectRatio() const
{
	return static_cast<float>(ClientWidth) / ClientHeight;
}

bool FD3DApp::Get4xMsaaState() const
{
	return X4MSAAState;
}

void FD3DApp::Set4xMsaaState(bool InValue)
{
	if (X4MSAAState != InValue)
	{
		X4MSAAState = InValue;
	
		CreateSwapChain();
		OnResize();
	}
}

int FD3DApp::Run()
{ 
	MSG msg = {};
	
	Timer.Reset();
	
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff
		else
		{
			Timer.Tick();
			if (!bAppPaused)
			{
				CalculateFrameStats();
				Update(Timer);
				Draw(Timer);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool FD3DApp::Initialize()
{
	if (!InitMainWindow())
	{
		return false;
	}
	
	if (!InitDirect3D())
	{
		return false;
	}
	OnResize();

	return true;
}


