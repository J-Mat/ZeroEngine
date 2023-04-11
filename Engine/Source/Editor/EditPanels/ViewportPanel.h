#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"
#include "Delegate.h"

namespace Zero
{
	DEFINITION_MULTICAST_DELEGATE(ResizeViewportEvent, void, uint32_t, uint32_t)

	class FViewportPanel :  public FBasePanel
	{
	public:
		FViewportPanel();
		void SetRenderTarget(Ref<FRenderTarget2D> RenderTarget, uint32_t Index = 0);
		void SetRenderTexture(Ref<FTexture2D> Texture) { m_RenderTexture = Texture; }
		bool IsViewportFocusd() { return m_bWindowsFocused; }
		bool IsViewportHovered() { return m_bWindowsHoverd; }
		virtual void OnGuiRender();
		void OnRenderGizmo();
		void AcceptDragDropEvent();
		virtual void OnMouseClick() override;
		ZMath::FRay GetProjectionRay();
		ResizeViewportEvent& GetResizeViewportEvent() { return m_ResizeViewportEvent; }
	private:
		bool m_bWindowsFocused;
		bool m_bWindowsHoverd;
		
		//Ref<FRenderTarget2D> m_RenderTarget;
		//uint32_t m_RenderTargetIndex = 0;

		ResizeViewportEvent m_ResizeViewportEvent;
		Ref<FTexture2D> m_RenderTexture = nullptr;
	};
}