#include "DX12SwapChain.h"

namespace Zero
{
	FDX12SwapChain::FDX12SwapChain(FDX12Device& Device, HWND hWnd, DXGI_FORMAT RenderTargetFormat)
		: FSwapChain()
		, m_Device(Device)
		, m_CommandQueue(m_Device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT))
		, m_hWnd(hWnd) 
		, m_RenderTargetFormat(RenderTargetFormat)
		, m_bTearingSupported(false)
	{
		auto D3DComandQueue = m_CommandQueue.GetD3DCommandQueue();
		
		// Query the factory from the adapter that was used to create the device.
		auto Adaptor = m_Device.GetAdapter();
		auto DxgiAdapter = Adaptor->GetDXGIAdapter();
		

		// Get the factory that was used to create the adapter.
		ComPtr<IDXGIFactory>  DxgiFactory;
		ComPtr<IDXGIFactory5> DxgiFactory5;
		ThrowIfFailed(DxgiAdapter->GetParent(IID_PPV_ARGS(&DxgiFactory)));
		// Now get the DXGIFactory5 so I can use the IDXGIFactory5::CheckFeatureSupport method.
		ThrowIfFailed(DxgiFactory.As(&DxgiFactory5));
		
		// Check for tearing support.
		BOOL AllowTearing = FALSE;
		if (SUCCEEDED(DxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &AllowTearing, sizeof(BOOL))))
		{
			m_bTearingSupported = (AllowTearing == TRUE);
		}
		
		// Query the windows client width and height.
		RECT WindowRect;
		::GetClientRect(hWnd, &WindowRect);
		
		m_Width = WindowRect.right - WindowRect.left;
		m_Height = WindowRect.bottom - WindowRect.top;
		
		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.Width = m_Width;
		SwapChainDesc.Height = m_Height;
		SwapChainDesc.Format = m_RenderTargetFormat;
		SwapChainDesc.Stereo = FALSE;
		SwapChainDesc.SampleDesc = { 1, 0 };
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = s_BufferCount;
		SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		// It is recommended to always allow tearing if tearing support is available.
		SwapChainDesc.Flags = m_bTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		
		// Now create the swap chain.
		ComPtr<IDXGISwapChain1> DxgiSwapChain1;
		ThrowIfFailed(DxgiFactory5->CreateSwapChainForHwnd(D3DComandQueue.Get(), m_hWnd, &SwapChainDesc, nullptr, nullptr, &DxgiSwapChain1));

		// Cast to swapchain4
		ThrowIfFailed(DxgiSwapChain1.As(&m_DxgiSwapChain));
		
		// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
		// will be handled manually.
		ThrowIfFailed(DxgiFactory5->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
		
		// Initialize the current back buffer index.
		m_CurrentBackBufferIndex = m_DxgiSwapChain->GetCurrentBackBufferIndex();

		// Set maximum frame latency to reduce input latency.
		m_DxgiSwapChain->SetMaximumFrameLatency(s_BufferCount - 1);

		// Get the SwapChain's waitable object.
		m_hFrameLatencyWaitableObject = m_DxgiSwapChain->GetFrameLatencyWaitableObject();
		
		UpdateRenderTargetViews();
	}

	FDX12SwapChain::~FDX12SwapChain()
	{
	}

	void FDX12SwapChain::UpdateRenderTargetViews()
	{
		for (UINT i = 0; i < s_BufferCount; ++i)
		{
			ComPtr<ID3D12Resource> BackBuffer;
			ThrowIfFailed(m_DxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffer)));
			
			m_BackBufferTextures[i] = CreateRef<FDX12Texture2D>(m_Device, BackBuffer);

			// Set the names for the backbuffer textures.
			// Useful for debugging.
			m_BackBufferTextures[i]->SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");

		}
	}
}
