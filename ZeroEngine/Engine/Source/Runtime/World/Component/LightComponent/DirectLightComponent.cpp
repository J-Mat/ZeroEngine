#include "DirectLightComponent.h"

namespace Zero
{
	UDirectLightComponnet::UDirectLightComponnet()
		: ULightComponent()
	{
	}
	
	void UDirectLightComponnet::PostInit()
	{
		ULightComponent::PostInit();
		FOnTransformChanged& OnTransformChanged = m_TransformCompent->GetTransformChange();
		OnTransformChanged.AddFunction(this, &UDirectLightComponnet::OnTransfromChanged);
	}

	ZMath::mat4 UDirectLightComponnet::GetProjectView()
	{
		return m_SceneView.ProjectionView;
	}

	void UDirectLightComponnet::OnTransfromChanged(UTransformComponent* TransformComponent)
	{
		ZMath::vec3 LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_SceneView.View = ZMath::lookAtLH(m_TransformCompent->m_Position, LookAt, ZMath::vec3(0.0f, 1.0f, 0.0f));
		m_SceneView.Proj = ZMath::orthoLH(-m_Width / 2.0f, m_Width / 2.0f, -m_Width / 2.0f, m_Width / 2.0f, 0.01f, 20.0f);
		m_SceneView.ProjectionView = m_SceneView.Proj * m_SceneView.View;
	}
}
