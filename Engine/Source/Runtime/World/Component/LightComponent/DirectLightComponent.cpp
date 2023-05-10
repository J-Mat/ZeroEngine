#include "DirectLightComponent.h"

namespace Zero
{
	UDirectLightComponnet::UDirectLightComponnet()
		: ULightComponent()
	{
		FSceneView& SceneView = m_SceneCapture2D.GetSceneView();
		SceneView.CameraType = ECameraType::CT_ORI;
	}
	
	void UDirectLightComponnet::PostInit()
	{
		ULightComponent::PostInit();
		FOnTransformChanged& OnTransformChanged = m_TransformCompent->GetTransformChange();
		OnTransformChanged.AddFunction(this, &UDirectLightComponnet::OnTransfromChanged);
		OnTransfromChanged(m_TransformCompent);
	}

	void UDirectLightComponnet::Tick()
	{
		Supper::Tick();
		m_SceneCapture2D.UpdateParams();
	}

	ZMath::mat4 UDirectLightComponnet::GetProjectView()
	{
		return m_SceneCapture2D.GetSceneView().ProjectionView;
	}

	void UDirectLightComponnet::OnTransfromChanged(UTransformComponent* TransformComponent)
	{
		static 	FSceneView& SceneView = m_SceneCapture2D.GetSceneView();
		SceneView.ViewPos = m_TransformCompent->m_Position;
		SceneView.ForwardDir = m_TransformCompent->m_ForwardVector;
		SceneView.UpDir = m_TransformCompent->m_UpVector;
		SceneView.RightDir = m_TransformCompent->m_RightVector;
		SceneView.Target = SceneView.ViewPos + SceneView.ForwardDir;

		SceneView.Left = -m_Width / 2.0f;
		SceneView.Right = m_Width / 2.0f;
		SceneView.Bottom = -m_Width / 2.0f;
		SceneView.Top = m_Width / 2.0f;
		SceneView.Near = 0.01f;
		SceneView.Far = 80.0f;

		//m_SceneCapture2D.UpdateParams();
		/*
		ZMath::vec3 LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_SceneView.View = ZMath::lookAtLH(m_TransformCompent->m_Position, LookAt, ZMath::vec3(0.0f, 1.0f, 0.0f));
		// z-axi is range of 0 - 1 in NDC of DX,
		m_SceneView.Proj = ZMath::orthoLH_ZO(-m_Width / 2.0f, m_Width / 2.0f, -m_Width / 2.0f, m_Width / 2.0f, 0.01f, 80.0f);
		m_SceneView.ProjectionView = m_SceneView.Proj * m_SceneView.View;
		*/
	}
}
