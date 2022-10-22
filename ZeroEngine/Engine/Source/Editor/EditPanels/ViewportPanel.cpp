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
		
		ImVec2 WindowsPos = ImGui::GetWindowPos();
		m_WindowsPos.x = int(WindowsPos.x);
		m_WindowsPos.y = int(WindowsPos.y);
		
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
	}


	ZMath::FRay FViewportPanel::GetProjectionRay()
	{
		UCameraActor* CameraActor = UWorld::GetCurrentWorld()->GetCameraActor();
		UCameraComponent* CameraComponent = CameraActor->GetComponent<UCameraComponent>();
		CLIENT_ASSERT(CameraComponent != nullptr, "CameraActor shoule be has CameraComponent");
		ZMath::mat4 Projection = CameraComponent->GetProjection();

		float ViewX = (+2.0f * m_MousePos.x / m_ViewportSize.x - 1.0f) / Projection[0][0];
		float ViewY = (-2.0f * m_MousePos.y / m_ViewportSize.y + 1.0f) / Projection[1][1];
		
		ZMath::vec3 m_Origin(0.0f);
		ZMath::vec3 m_Direction(ViewX, ViewY, 1.0f);
		
		return { m_Origin, m_Direction };
	}

	void FViewportPanel::OnMouseClick(int X, int Y)
	{
		m_MousePos = { X - m_WindowsPos.x, Y - m_WindowsPos.y };
		ZMath::FRay Ray = GetProjectionRay();
		UActor* Actor = UWorld::GetCurrentWorld()->PickActorByMouse(Ray);
		if (Actor != nullptr)
		{
			std::cout << Actor->GetName() << std::endl;
		}
	}


}
