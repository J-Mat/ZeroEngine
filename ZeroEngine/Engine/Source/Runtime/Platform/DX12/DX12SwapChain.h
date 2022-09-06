#pragma once

#include "Core.h"
#include "../DX12/Common/DX12Header.h"
#include "Render/RHI/SwapChain.h"
#include "DX12RenderTarget.h"
#include "DX12CommandQueue.h"
#include <dxgi1_5.h>

namespace Zero
{
	class FDX12Texture2D;
	class FDX12Device;
	class FDX12SwapChain : public FSwapChain
	{
	public:	 
		static const UINT s_BufferCount = 3;
		
		FDX12SwapChain(FDX12Device& Device, HWND hWnd, DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
		virtual ~FDX12SwapChain();

        /**
		* Check to see if tearing is supported.
		*
		* @see https://docs.microsoft.com/en-us/windows/win32/direct3ddxgi/variable-refresh-rate-displays
		*/
        bool IsTearingSupported() const { return m_bTearingSupported; }
		virtual void Resize(uint32_t Width, uint32_t Height);
		void SetFullScreen(bool bFullScreen);
		void WaitForSwapChain();
		const Ref<FDX12RenderTarget> GetRenderTarget() const;
		UINT Present(const Ref<FDX12Texture2D>& Texture = nullptr);
		
	private:
		void UpdateRenderTargetViews();
		
	private:
		FDX12Device& m_Device;
		FDX12CommandQueue& m_CommandQueue;
		ComPtr<IDXGISwapChain4> m_DxgiSwapChain;
		Ref<FDX12Texture2D> m_BackBufferTextures[s_BufferCount];
		mutable Ref<FDX12RenderTarget>  m_RenderTarget;

		// The current backbuffer index of the swap chain.	
		UINT m_CurrentBackBufferIndex;
		UINT64 m_FenceValues[s_BufferCount];  // The fence values to wait for before leaving the Present method.
		
		// A handle to a waitable object. Used to wait for the swapchain before presenting.
		HANDLE m_hFrameLatencyWaitableObject;

		// The window handle that is associates with this swap chain.
		HWND m_hWnd;

		// The format of the back buffer.
		DXGI_FORMAT m_RenderTargetFormat;
		
	protected:
		// Whether or not tearing is supported.
		bool m_bTearingSupported;
	};
}
