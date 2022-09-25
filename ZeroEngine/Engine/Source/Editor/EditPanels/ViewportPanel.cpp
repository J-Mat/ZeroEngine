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
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2({ 0,0 }));
		//ImGui::Begin("Viewport", 0, ImGuiWindowFlags_MenuBar);
		//ImGui::Begin("Viewport", 0, ImGuiWindowFlags_MenuBar);

		m_bViewportFocused = ImGui::IsWindowFocused();
		m_bViewportHoverd = ImGui::IsWindowHovered();

		ImVec2 ViewportPanelSize = ImGui::GetContentRegionAvail();
		/*
		if (m_ViewportSize != *((ZMath::vec2*)&ViewportPanelSize))
		{
			m_ViewportSize = { ViewportPanelSize.x, ViewportPanelSize.y };
			if (m_RenderTarget != nullptr)
			{
				m_RenderTarget->Resize(m_ViewportSize.x, m_ViewportSize.y);
			}
		}
		*/
		
		Ref<FRenderTarget> RenderTarget = TLibrary<FRenderTarget>::Fetch(FORWARD_STAGE);
		Ref<FTexture2D> Texture = RenderTarget->GetTexture(m_RenderTargetIndex);
		ImGui::Begin("Texture Test");
		ImGui::Text("size = %d x %d", Texture->GetWidth(), Texture->GetHeight());
		ImVec2 vec = {float(Texture->GetWidth()), float(Texture->GetHeight())};
		ImGui::Image((ImTextureID)Texture->GetGuiShaderReseource(), vec);
		ImGui::End();
	}
}
