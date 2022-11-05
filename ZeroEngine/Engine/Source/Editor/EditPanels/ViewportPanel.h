#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"


namespace Zero
{
	class FViewportPanel :  public FBasePanel
	{
	public:
		FViewportPanel() = default;
		void SetRenderTarget(Ref<FRenderTarget> RenderTarget, EAttachmentIndex Index = EAttachmentIndex::Color0);
		bool IsViewportFocusd() { return m_bWindowsFocused; }
		bool IsViewportHovered() { return m_bWindowsHoverd; }
		virtual void OnGuiRender();
		void OnRenderGizmo();
		void AcceptDragDropEvent();
		virtual void OnMouseClick() override;
		ZMath::FRay GetProjectionRay();
	private:
		bool m_bWindowsFocused;
		bool m_bWindowsHoverd;
		
		Ref<FRenderTarget> m_RenderTarget;
		EAttachmentIndex m_RenderTargetIndex = EAttachmentIndex::Color0;
	};
}