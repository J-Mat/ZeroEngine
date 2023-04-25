#pragma once

#include <ZeroEngine.h>
#include "BasePanel.h"


namespace Zero
{
	class FDebugViewportPanel :  public FBasePanel
	{
	public:
		FDebugViewportPanel() = default;
		void SetTextures(Ref<FTexture2D> Texture);
		bool IsViewportFocusd() { return m_bWindowsFocused; }
		bool IsViewportHovered() { return m_bWindowsHoverd; }
		virtual void OnGuiRender();
	private:
		bool m_bWindowsFocused;
		bool m_bWindowsHoverd;
		
		Ref<FTexture2D> m_Texture;
		uint32_t m_RenderTargetIndex = 0;
	};
}