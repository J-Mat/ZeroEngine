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
		ComPtr<IDXGIFactory4>  DxgiFactory;
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));
		
		// Query the windows client width and height.
		RECT WindowRect;
		::GetClientRect(hWnd, &WindowRect);
		
		m_Width = WindowRect.right - WindowRect.left;
		m_Height = WindowRect.bottom - WindowRect.top;
		
		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		SwapChainDesc.BufferDesc.Width = m_Width;
		SwapChainDesc.BufferDesc.Height = m_Height;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		SwapChainDesc.SampleDesc.Count = 1;
		SwapChainDesc.SampleDesc.Quality = 0;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = s_BufferCount;
		SwapChainDesc.OutputWindow = m_hWnd;
		SwapChainDesc.Windowed = true;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		
		ThrowIfFailed(
			DxgiFactory->CreateSwapChain(
				D3DComandQueue.Get(),
				&SwapChainDesc,
				m_DxgiSwapChain.GetAddressOf()
			)
		);
		
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
			m_DepthStencilTexture.reset();
		
			DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
			ThrowIfFailed(m_DxgiSwapChain->GetDesc(&SwapChainDesc));
			ThrowIfFailed(m_DxgiSwapChain->ResizeBuffers(s_BufferCount, m_Width, m_Height, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags));
			
			m_CurrentBackBufferIndex = 0;
			
			UpdateRenderTargetViews();
			
			m_RenderTarget = CreateRef<FDX12RenderTarget>(m_Device);
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

	const Ref<FRenderTarget> FDX12SwapChain::GetRenderTarget()
	{
		m_RenderTarget->AttachTexture(EAttachmentIndex::Color0, m_BackBufferTextures[m_CurrentBackBufferIndex]);
		m_RenderTarget->AttachTexture(EAttachmentIndex::DepthStencil, m_DepthStencilTexture);
		return m_RenderTarget;
	}

	UINT FDX12SwapChain::Present(Ref<FTexture2D> Texture)
	{
		auto CommandList = m_Device.GetRenderCommandList();
		
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
	
		CommandList->TransitionBarrier(BufferBuffer, D3D12_RESOURCE_STATE_PRESENT);
		m_CommandQueue.ExecuteCommandList(CommandList);

		ThrowIfFailed(m_DxgiSwapChain->Present(0, 0));

		m_FenceValues[m_CurrentBackBufferIndex] = m_CommandQueue.Signal();

		m_CurrentBackBufferIndex = (m_CurrentBackBufferIndex + 1) % s_BufferCount;

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
		
		auto DepthTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Width, m_Height);
		// Must be set on textures that will be used as a depth-stencil buffer.
		DepthTextureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// Specify optimized clear values for the depth buffer.
		D3D12_CLEAR_VALUE OptimizedClearValue = {};
		OptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		OptimizedClearValue.DepthStencil = { 1.0F, 0 };
		m_DepthStencilTexture = CreateRef<FDX12Texture2D>(m_Device, DepthTextureDesc, &OptimizedClearValue);
	}
}
