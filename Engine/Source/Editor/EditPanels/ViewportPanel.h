#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"


namespace Zero
{
	class FViewportPanel :  public FBasePanel
	{
	public:
		FViewportPanel() = default;
		void SetRenderTarget(Ref<FRenderTarget2D> RenderTarget, uint32_t Index = 0);
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
		
		Ref<FRenderTarget2D> m_RenderTarget;
		uint32_t m_RenderTargetIndex = 0;
	};
}