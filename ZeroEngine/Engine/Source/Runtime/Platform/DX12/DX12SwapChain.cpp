#include "DX12SwapChain.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include <dxgi1_2.h>

namespace Zero
{
	FDX12SwapChain::FDX12SwapChain(FDX12Device& Device, HWND hWnd, DXGI_FORMAT RenderTargetFormat)
		: FSwapChain()
		, m_Device(Device)
		, m_CommandQueue(m_Device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT))
		, m_hWnd(hWnd) 
		, m_RenderTargetFormat(RenderTargetFormat)
		, m_bTearingSupported(false)
		, m_FenceValues {0}
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

		m_RenderTarget = CreateRef<FDX12RenderTarget>(m_Device);
	}

	FDX12SwapChain::~FDX12SwapChain()
	{
	}

	void FDX12SwapChain::Resize(uint32_t Width, uint32_t Height)
	{
		if (m_Width != Width || m_Height != Height)
		{
			m_Width = std::max(Width, 1u);
			m_Height = std::max(Height, 1u);

			m_Device.Flush();
			
			m_RenderTarget->Reset();
			for (UINT i = 0; i < s_BufferCount; ++i)
			{
				m_BackBufferTextures[i].reset();
			}
		
			DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
			ThrowIfFailed(m_DxgiSwapChain->GetDesc(&SwapChainDesc));
			ThrowIfFailed(m_DxgiSwapChain->ResizeBuffers(s_BufferCount, m_Width, m_Height, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags));
			
			m_CurrentBackBufferIndex = m_DxgiSwapChain->GetCurrentBackBufferIndex();
			
			UpdateRenderTargetViews();
		}
	}

	void FDX12SwapChain::SetFullScreen(bool bFullScreen)
	{
		if (m_bFullScreen != bFullScreen)
		{
			m_bFullScreen = bFullScreen;
		}
	}

	void FDX12SwapChain::WaitForSwapChain()
	{
		DWORD result = ::WaitForSingleObjectEx(m_hFrameLatencyWaitableObject, 1000, TRUE);  // Wait for 1 second (should never have to wait that long...)
	}

	const Ref<FDX12RenderTarget> FDX12SwapChain::GetRenderTarget() const
	{
		m_RenderTarget->AttachTexture(EAttachmentIndex::Color0, m_BackBufferTextures[m_CurrentBackBufferIndex]);
		return m_RenderTarget;
	}

	UINT FDX12SwapChain::Present(const Ref<FDX12Texture2D>& Texture)
	{
		auto CommandList = m_CommandQueue.GetCommandList();
		
		Ref<FDX12Texture2D> BufferBuffer = m_BackBufferTextures[m_CurrentBackBufferIndex];
		
		if (Texture)
		{
			if (Texture->GetD3D12ResourceDesc().SampleDesc.Count > 1)
			{
				CommandList->ResolveSubResource(BufferBuffer, Texture);
			}
			else
			{
				CommandList->CopyResource(BufferBuffer, Texture);
			}
		}
	
		CommandList->TransitionBarrier(BufferBuffer, D3D12_RESOURCE_STATE_PRESENT);
		m_CommandQueue.ExecuteCommandList(CommandList);

		UINT SyncInterval = m_bVSync ? 1 : 0;
		UINT PresentFlags = m_bTearingSupported && !m_bFullScreen && !m_bVSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		ThrowIfFailed(m_DxgiSwapChain->Present(SyncInterval, PresentFlags));

		m_FenceValues[m_CurrentBackBufferIndex] = m_CommandQueue.Signal();

		m_CurrentBackBufferIndex = m_DxgiSwapChain->GetCurrentBackBufferIndex();

		auto FenceValue = m_FenceValues[m_CurrentBackBufferIndex];
		m_CommandQueue.WaitForFenceValue(FenceValue);
		
		m_Device.ReleaseStaleDescriptors();

		return m_CurrentBackBufferIndex;
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
