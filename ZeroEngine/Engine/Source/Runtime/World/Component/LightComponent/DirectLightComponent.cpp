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
		OnTransfromChanged(m_TransformCompent);
	}

	ZMath::mat4 UDirectLightComponnet::GetProjectView()
	{
		return m_SceneView.ProjectionView;
	}

	void UDirectLightComponnet::OnTransfromChanged(UTransformComponent* TransformComponent)
	{
		ZMath::vec3 LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_SceneView.View = ZMath::lookAtLH(m_TransformCompent->m_Position, LookAt, ZMath::vec3(0.0f, 1.0f, 0.0f));
		// z-axi is range of 0 - 1 in NDC of DX,
		m_SceneView.Proj = ZMath::orthoLH_ZO(-m_Width / 2.0f, m_Width / 2.0f, -m_Width / 2.0f, m_Width / 2.0f, 0.01f, 80.0f);
		m_SceneView.ProjectionView = m_SceneView.Proj * m_SceneView.View;
	}
}
