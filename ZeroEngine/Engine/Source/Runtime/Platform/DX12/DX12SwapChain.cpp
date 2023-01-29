#include "DX12SwapChain.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include <dxgi1_2.h>
#include "Core/Framework/Library.h"

namespace Zero
{
	FDX12SwapChain::FDX12SwapChain(HWND hWnd, DXGI_FORMAT RenderTargetFormat)
		: FSwapChain()
		, m_CommandQueue(FDX12Device::Get()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT))
		, m_hWnd(hWnd) 
		, m_RenderTargetFormat(RenderTargetFormat)
		, m_bTearingSupported(false)
		, m_FenceValues {0}
	{
		auto D3DComandQueue = m_CommandQueue.GetD3DCommandQueue();
		
		// Query the factory from the adapter that was used to create the device.
		auto Adaptor = FDX12Device::Get()->GetAdapter();
		auto DxgiAdapter = Adaptor->GetDXGIAdapter();
		

		// Get the factory that was used to create the adapter.
		ComPtr<IDXGIFactory>  DxgiFactory;
		ComPtr<IDXGIFactory5>  DxgiFactory5;
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));
		ThrowIfFailed(DxgiFactory.As(&DxgiFactory5));

		// Check for tearing support.
		BOOL allowTearing = FALSE;
		if (SUCCEEDED(
			DxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL))))
		{
			m_bTearingSupported = (allowTearing == TRUE);
		}
		
		// Query the windows client width and height.
		RECT WindowRect;
		::GetClientRect(hWnd, &WindowRect);
		
		m_Width = WindowRect.right - WindowRect.left;
		m_Height = WindowRect.bottom - WindowRect.top;
		
		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc;
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
		SwapChainDesc.Flags =  m_bTearingSupported? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		
		// Now create the swap chain.
		ComPtr<IDXGISwapChain1> dxgiSwapChain1;
		ThrowIfFailed(DxgiFactory5->CreateSwapChainForHwnd(m_CommandQueue.GetD3DCommandQueue().Get(), m_hWnd, &SwapChainDesc, nullptr,
			nullptr, &dxgiSwapChain1));
		
		// Cast to swapchain4
		ThrowIfFailed(dxgiSwapChain1.As(&m_DxgiSwapChain));


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

		m_RenderTarget = CreateRef<FDX12RenderTarget2D>();
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

			FDX12Device::Get()->Flush();
		
			m_RenderTarget->Reset();
			for (UINT i = 0; i < s_BufferCount; ++i)
			{
				m_BackBufferTextures[i].reset();
			}
			m_DepthStencilTexture.reset();
		
			DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
			ThrowIfFailed(m_DxgiSwapChain->GetDesc(&SwapChainDesc));
			ThrowIfFailed(m_DxgiSwapChain->ResizeBuffers(s_BufferCount, m_Width, m_Height, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags));
			
			m_CurrentBackBufferIndex = 0;
			
			UpdateRenderTargetViews();
			
			m_RenderTarget = CreateRef<FDX12RenderTarget2D>();
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

	const Ref<FRenderTarget2D> FDX12SwapChain::GetRenderTarget()
	{
		m_RenderTarget->AttachTexture(EAttachmentIndex::Color0, m_BackBufferTextures[m_CurrentBackBufferIndex]);
		m_RenderTarget->AttachTexture(EAttachmentIndex::DepthStencil, m_DepthStencilTexture);
		TLibrary<FRenderTarget2D>::Push("MainViewport", m_RenderTarget);
		return m_RenderTarget;
	}

	void FDX12SwapChain::SetRenderTarget()
	{
		GetRenderTarget()->Bind();
	}

	UINT FDX12SwapChain::Present(Ref<FTexture2D> Texture)
	{
		auto CommandList = FDX12Device::Get()->GetRenderCommandList();
		
		Ref<FDX12Texture2D> BufferBuffer = m_BackBufferTextures[m_CurrentBackBufferIndex];
		
		/*
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
		*/
	
		CommandList->TransitionBarrier(BufferBuffer->GetD3DResource(), D3D12_RESOURCE_STATE_PRESENT);
		m_CommandQueue.ExecuteCommandList(CommandList);


		bool VSync = false;
		bool m_bFullscreen = false;
		UINT syncInterval = VSync ? 1 : 0;
		UINT presentFlags = m_bTearingSupported  && !m_bFullscreen && !VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;

		ThrowIfFailed(m_DxgiSwapChain->Present(syncInterval, presentFlags));

		m_FenceValues[m_CurrentBackBufferIndex] = m_CommandQueue.Signal();

		m_CurrentBackBufferIndex = (m_CurrentBackBufferIndex + 1) % s_BufferCount;

		auto FenceValue = m_FenceValues[m_CurrentBackBufferIndex];
		m_CommandQueue.WaitForFenceValue(FenceValue);
		
		FDX12Device::Get()->ReleaseStaleDescriptors();

		return m_CurrentBackBufferIndex;
	}

	void FDX12SwapChain::UpdateRenderTargetViews()
	{
		for (UINT i = 0; i < s_BufferCount; ++i)
		{
			ComPtr<ID3D12Resource> BackBuffer;
			ThrowIfFailed(m_DxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffer)));
			
			std::string BufferName = std::format("BackBuffer[{0}]", i);
			m_BackBufferTextures[i] = CreateRef<FDX12Texture2D>(BufferName, BackBuffer, m_Width, m_Height);
			m_BackBufferTextures[i]->RegistGuiShaderResource();

			// Set the names for the backbuffer textures.
			// Useful for debugging.
		}
		
		
		// Resize the depth texture.
		auto DepthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, m_Width, m_Height);
		// Must be set on textures that will be used as a depth-stencil buffer.
		DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// Specify optimized clear values for the depth buffer.
		D3D12_CLEAR_VALUE OptClear = {};
		OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		OptClear.DepthStencil = { 1.0F, 0 };
		std::string DepthStencilTexture = "DepthStencilTexture";
		FDX12TextureSettings Settings
		{
			.Desc = DepthStencilDesc,
		};
		m_DepthStencilTexture = CreateRef<FDX12Texture2D>(DepthStencilTexture, Settings, &OptClear);
	}
}
