#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderTarget2D;
	class FTexture2D;
	struct FCommandListHandle;
	class FSwapChain
	{
	public:
		static const UINT s_BufferCount = 2;
		FSwapChain(uint32_t Width = 1, uint32_t Height = 1) 
			: m_Width(Width), m_Height(Height)
		{}
		virtual ~FSwapChain() { }

		virtual void Resize(uint32_t Width, uint32_t Hegiht) {}
		virtual void BindRenderTarget() {}
		virtual void SetRenderTarget(FCommandListHandle CommandListHandle) = 0;
		virtual void PreparePresent() {}
		virtual UINT Present(Ref<FTexture2D> Texture = nullptr) = 0;

		/**
		* Check to see if the swap chain is in full-screen exclusive mode.
		*/
		virtual bool IsFullScreen() const { return m_bFullScreen;}

		virtual void SetVSync(bool bVSync) { m_bVSync = bVSync;}
		virtual const Ref<FRenderTarget2D> GetRenderTarget() = 0;

		bool GetVSync() const { return m_bVSync; }

		virtual void Reisze(uint32_t width, uint32_t height) {}
		
		std::pair<uint32_t, uint32_t> GetSize() { return { m_Width, m_Height }; }
		uint32_t GetCurrentBufferIndex() { return m_CurrentBackBufferIndex; }

	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		bool m_bFullScreen = false;
		bool m_bVSync = false;
		UINT m_CurrentBackBufferIndex = 0;
	};
}
