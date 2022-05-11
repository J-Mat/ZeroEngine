#include "d3dApp.h"

FD3DApp* FD3DApp::App = nullptr;



LRESULT FD3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	default:
		break;
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
		std::string Text = 
		 	"Width = " + std::to_string(Model.Width) + " " +
            "Height = " + std::to_string(Model.Height) + " " +
            "Refresh = " + std::to_string(N) + "/" + std::to_string(D) +
            "\n";
		::OutputDebugString((LPCSTR)Text.c_str());
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
		::OutputDebugString((LPCSTR)Text.c_str());
		
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
		
		OutputDebugString((LPCSTR)Text.c_str());
		
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
	ThrowIfFailed(D3DDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommanQueue)));
	
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
			CommanQueue.Get(),
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
			IID_PPV_ARGS(RtvHeap.GetAddressOf()))
	);
	
	
	D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc;
	DsvHeapDesc.NumDescriptors = 1;
	DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;	
	DsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(
		D3DDevice->CreateDescriptorHeap(
			&DsvHeapDesc, 
			IID_PPV_ARGS(DsvHeap.GetAddressOf()))
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
		CommanQueue->Signal(Fence.Get(), CurrentFence)
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

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return FD3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}


FD3DApp::FD3DApp(HINSTANCE HInstance)
{
	FD3DApp::App = this;
}

FD3DApp::~FD3DApp()
{

}


bool FD3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = AppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return false;
	}
	
	RECT R = { 0, 0, ClientWidth, ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	
	int width = R.right - R.left;
	int height = R.bottom - R.top;
	MainWnd = CreateWindow("MainWindow", "d3d App",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, AppInstance, 0);
	
	if (MainWnd == nullptr)
	{
		MessageBox(0, "CreateWindow Failed.", 0, 0);
		return false;
	}
	
	ShowWindow(MainWnd, SW_SHOW);
	UpdateWindow(MainWnd);
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

	ThrowIfFailed(
		SwapChain->ResizeBuffers(
			SwapChainBufferCount,
			ClientWidth, ClientHeight,
			BackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		)
	);
	
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


