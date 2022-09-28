#include "ViewportPanel.h"

namespace Zero
{
	void FViewportPanel::SetRenderTarget(Ref<FRenderTarget> RenderTarget, EAttachmentIndex Index)
	{
		m_RenderTarget = RenderTarget;
		m_RenderTargetIndex = Index;
	}
	void FViewportPanel::OnGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2({ 0,0 }));
		ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar);

		m_bViewportFocused = ImGui::IsWindowFocused();
		m_bViewportHoverd = ImGui::IsWindowHovered();
		
		ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
		ViewportPanelSize.x = ZMath::max(ViewportPanelSize.x, 1600.0f / 2.0f);
		ViewportPanelSize.y = ZMath::max(ViewportPanelSize.y, 900.0f /2.0f);
		
		if (m_ViewportSize != *((ZMath::vec2*)&ViewportPanelSize))
		{
			m_ViewportSize = { ViewportPanelSize.x, ViewportPanelSize.y };
			if (m_RenderTarget != nullptr)
			{
				m_RenderTarget->Resize(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
			}
			
			UWorld::GetCurrentWorld()->GetCameraActor()->OnResizeViewport(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
		}
		
		Ref<FTexture2D> Texture = m_RenderTarget->GetTexture(m_RenderTargetIndex);
		ImGui::Image((ImTextureID)Texture->GetGuiShaderReseource(), ViewportPanelSize);
		ImGui::End();
		ImGui::PopStyleVar();
		
		ZMath::b
	}
}
