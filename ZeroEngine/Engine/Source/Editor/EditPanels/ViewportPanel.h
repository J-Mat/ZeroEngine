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
		void OnMouseClick(int X, int Y);
		ZMath::FRay GetProjectionRay();
	private:
		bool m_bViewportFocused;
		bool m_bViewportHoverd;
		ZMath::ivec2 m_WindowsPos;
		ZMath::ivec2 m_MousePos;
		ZMath::vec2 m_ViewportSize;

		Ref<FRenderTarget> m_RenderTarget;
		EAttachmentIndex m_RenderTargetIndex = EAttachmentIndex::Color0;
	};
}