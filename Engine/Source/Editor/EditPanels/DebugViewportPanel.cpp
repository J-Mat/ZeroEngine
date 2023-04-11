#include "DebugViewportPanel.h"
#include "Editor.h"

namespace Zero
{
	void FDebugViewportPanel::SetRenderTarget(Ref<FRenderTarget2D> RenderTarget, uint32_t Index)
	{
		m_RenderTarget = RenderTarget;
		m_RenderTargetIndex = Index;
	}
	void FDebugViewportPanel::OnGuiRender()
	{
		return;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2({ 0,0 }));
		ImGui::Begin("Debug", 0, ImGuiWindowFlags_MenuBar);
		
		PreInitWindow();

		m_WindowsSize.x = ZMath::max(m_WindowsSize.x, int(500.0f / 2.0f));
		m_WindowsSize.y = ZMath::max(m_WindowsSize.y, int(500.0f /2.0f));	
		ImGui::SetWindowSize("Debug",ImVec2(300, 300));
		//ImVec2 ViewportPanelSize = { float(m_WindowsSize.x), float(m_WindowsSize.y) };
		
		ImVec2 ViewportPanelSize = { float(300), float(300) };

		/*
		if (m_WindowsSize != m_LastWindowsSize)
		{
			if (m_RenderTarget != nullptr)
			{
				m_RenderTarget->Resize(uint32_t(m_WindowsSize.x), uint32_t(m_WindowsSize.y));
			}
		}
		*/
		
		FTexture2D* Texture = m_RenderTarget->GetColorTexture(m_RenderTargetIndex);
		ImGui::Image((ImTextureID)Texture->GetGuiShaderReseource(), ViewportPanelSize);

		ImGui::End();
		ImGui::PopStyleVar();
	}
}
