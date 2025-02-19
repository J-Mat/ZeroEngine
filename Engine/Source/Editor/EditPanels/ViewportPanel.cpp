#include "ViewportPanel.h"
#include "Editor.h"
#include "World/Actor/CustomMeshActor.h"

namespace Zero
{

	FViewportPanel::FViewportPanel():
		FBasePanel()
	{
		m_WindowsSize = { 800.0f, 450.0f };
	}

	void FViewportPanel::SetRenderTarget(Ref<FRenderTarget2D> RenderTarget, uint32_t Index)
	{
		//m_RenderTarget = RenderTarget;
		//m_RenderTargetIndex = Index;
	}

	void FViewportPanel::OnGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2({ 0,0 }));
		ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar);
		
		PreInitWindow();

		m_WindowsSize.x = ZMath::max(m_WindowsSize.x, int(1600.0f / 2.0f));
		m_WindowsSize.y = ZMath::max(m_WindowsSize.y, int(900.0f /2.0f));	
		ImVec2 ViewportPanelSize = { float(m_WindowsSize.x), float(m_WindowsSize.y) };
		if (m_WindowsSize != m_LastWindowsSize)
		{
			/*
			if (m_RenderTarget != nullptr)
			{
				m_RenderTarget->Resize(uint32_t(m_WindowsSize.x), uint32_t(m_WindowsSize.y));
				m_ResizeViewportEvent.Broadcast(uint32_t(m_WindowsSize.x), uint32_t(m_WindowsSize.y));
			}
			*/
			m_ResizeViewportEvent.Broadcast(uint32_t(m_WindowsSize.x), uint32_t(m_WindowsSize.y));
			//m_RenderTexture = nullptr;
			
			UWorld::GetCurrentWorld()->GetMainCamera()->OnResizeViewport(uint32_t(m_WindowsSize.x), uint32_t(m_WindowsSize.y));
		}
		
		if (m_RenderTexture)
		{
			ImGui::Image((ImTextureID)m_RenderTexture->GetGuiShaderReseource(), ViewportPanelSize);
		}

		OnMouseClick();
		OnRenderGizmo();
		AcceptDragDropEvent();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void FViewportPanel::OnRenderGizmo()
	{
		if (FEditor::SelectedActor == nullptr)
		{
			return;
		}

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(float(m_WindowsPos.x), float(m_WindowsPos.y), float(m_WindowsSize.x), float(m_WindowsSize.y));

		
		UCameraActor* CameraActor = UWorld::GetCurrentWorld()->GetMainCamera();
		UCameraComponent* CameraComponent = CameraActor->GetComponent<UCameraComponent>();
		CLIENT_ASSERT(CameraComponent != nullptr, "CameraActor shoule be has CameraComponent");
		const auto& SceneView = CameraComponent->GetSceneView();
		ZMath::mat4 CameraProjection = SceneView.Proj;
		ZMath::mat4 CameraView = SceneView.View;
		ZMath::mat4 Transform = FEditor::SelectedActor->GetTransform();

		static std::vector<ImGuizmo::OPERATION> VecOperation =
		{ 
			ImGuizmo::OPERATION::TRANSLATE,
			ImGuizmo::OPERATION::ROTATE,
			ImGuizmo::OPERATION::SCALE,
			ImGuizmo::OPERATION::SCALEU,
			ImGuizmo::OPERATION::UNIVERSAL
		};
		static int OperationIndex = 0;
		if (ImGui::IsKeyPressed(ImGuiKey_::ImGuiKey_M))
			OperationIndex = (OperationIndex + 1) % VecOperation.size();

		ImGuizmo::Manipulate(ZMath::value_ptr(CameraView), ZMath::value_ptr(CameraProjection),
			VecOperation[OperationIndex], ImGuizmo::LOCAL, ZMath::value_ptr(Transform),
			nullptr, nullptr);
		
		if (ImGuizmo::IsUsing())
		{
			auto* TransformeComponent = FEditor::SelectedActor->GetComponent<UTransformComponent>();
			TransformeComponent->SetTransfrom(Transform);
		}
	}

	void FViewportPanel::AcceptDragDropEvent()
	{
		UWorld* World = UWorld::GetCurrentWorld();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(ASSET_PANEL_OBJ))
			{
				std::string Path = (const char*)Payload->Data;
				ZMath::FRay Ray = GetProjectionRay();
				ZMath::vec3 Pos = World->GetRayWorldPos(Ray, 5.0f);
				UCustomMeshActor* Actor = World->CreateActor<UCustomMeshActor>(Path);
				Actor->SetPosition(Pos);
			}
			else if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(PLACEOBJ_PANEL))
			{
				std::string ActorName = (const char*)Payload->Data;

				CLIENT_LOG_TRACE("Actor Name '{0}'", ActorName);
				auto Iter = FObjectGlobal::GetClassInfoMap().find(ActorName);
				if (Iter != FObjectGlobal::GetClassInfoMap().end())
				{
					UActor* Actor = nullptr;
					Actor = static_cast<UActor*>(Iter->second.Class->CreateDefaultObject());
					if (Actor != nullptr)
					{
						Actor->SetWorld(World);
						Actor->InitReflectionContent();
						Actor->PostInit();
						World->AddActor(Actor);
						ZMath::FRay Ray = GetProjectionRay();
						ZMath::vec3 Pos = World->GetRayWorldPos(Ray, 5.0f);
						Actor->SetPosition(Pos);
					}
				}
			}
		
			ImGui::EndDragDropTarget();
		}
	}

	ZMath::FRay FViewportPanel::GetProjectionRay()
	{
		UCameraActor* CameraActor = UWorld::GetCurrentWorld()->GetMainCamera();
		UCameraComponent* CameraComponent = CameraActor->GetComponent<UCameraComponent>();
		CLIENT_ASSERT(CameraComponent != nullptr, "CameraActor shoule be has CameraComponent");
		const auto& SceneView = CameraComponent->GetSceneView();
		ZMath::mat4 Projection = SceneView.Proj;

		float ViewX = (+2.0f * m_MouseLocalPos.x / m_WindowsSize.x - 1.0f) / Projection[0][0];
		float ViewY = (-2.0f * m_MouseLocalPos.y / m_WindowsSize.y + 1.0f) / Projection[1][1];
		
		ZMath::vec3 m_Origin(0.0f);
		ZMath::vec3 m_Direction(ViewX, ViewY, 1.0f);
		
		return { m_Origin, m_Direction };
	}

	void FViewportPanel::OnMouseClick()
	{
		if (MouseButtonReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			ZMath::FRay Ray = GetProjectionRay();
			UActor* Actor = UWorld::GetCurrentWorld()->PickActorByMouse(Ray);
			if (Actor != nullptr)
			{
				FEditor::SelectedActor = Actor;
			}
			else
			{
				FEditor::SelectedActor = nullptr;
			}
		}
	}
}
