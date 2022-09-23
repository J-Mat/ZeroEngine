#pragma once

#include <ZeroEngine.h>


namespace Zero
{
	class FViewportPanel
	{
	public:
		FViewportPanel() = default;
		void SetRenderTarget(Ref<FRenderTarget> RenderTarget, EAttachmentIndex Index = EAttachmentIndex::Color0);
		bool IsViewportFocusd() { return m_bViewportFocused; }
		bool IsViewportHovered() { return m_bViewportHoverd; }
		void OnGuiRender();
	private:
		bool m_bViewportFocused;
		bool m_bViewportHoverd;
		ZMath::vec2 m_ViewportSize;

		Ref<FRenderTarget> m_RenderTarget;
		EAttachmentIndex m_RenderTargetIndex = EAttachmentIndex::Color0;
	};
}