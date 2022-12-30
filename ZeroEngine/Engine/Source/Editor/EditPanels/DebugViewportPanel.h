#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"


namespace Zero
{
	class FDebugViewportPanel :  public FBasePanel
	{
	public:
		FDebugViewportPanel() = default;
		void SetRenderTarget(Ref<FRenderTarget2D> RenderTarget, EAttachmentIndex Index = EAttachmentIndex::Color0);
		bool IsViewportFocusd() { return m_bWindowsFocused; }
		bool IsViewportHovered() { return m_bWindowsHoverd; }
		virtual void OnGuiRender();
	private:
		bool m_bWindowsFocused;
		bool m_bWindowsHoverd;
		
		Ref<FRenderTarget2D> m_RenderTarget;
		EAttachmentIndex m_RenderTargetIndex = EAttachmentIndex::Color0;
	};
}