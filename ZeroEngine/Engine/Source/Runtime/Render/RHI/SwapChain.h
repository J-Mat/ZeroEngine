#pragma once
#include "PCH.h"

namespace Zero
{
	class FSwapChain
	{
	public:
		FSwapChain(uint32_t Width = 1, uint32_t Height = 1) 
			: m_Width(Width), m_Height(Height)
		{}
		virtual ~FSwapChain() { }

		virtual void Resize(uint32_t Width, uint32_t Hegiht) {}
		virtual void BindRenderTarget() {}
		virtual void SetRenderTarget() {}
		virtual void PreparePresent() {}
		virtual void Present() {}

		/**
		* Check to see if the swap chain is in full-screen exclusive mode.
		*/
		virtual bool IsFullScreen() const { return m_bFullScreen;}

		virtual void SetVSync(bool bVSync) { m_bVSync = bVSync;}

		bool GetVSync() const { return m_bVSync; }

		virtual void Reisze(uint32_t width, uint32_t height) {}

	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		bool m_bFullScreen = false;
		bool m_bVSync = false;
	};
}
