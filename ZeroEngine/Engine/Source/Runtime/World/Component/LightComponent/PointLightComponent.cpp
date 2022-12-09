#include "PointLightComponent.h"

namespace Zero
{
	void UPointLightComponnet::PostInit()
	{
		ULightComponent::PostInit();
		FOnTransformChanged& OnTransformChanged = m_TransformCompent->GetTransformChange();
		OnTransformChanged.AddFunction(this, &UPointLightComponnet::OnTransfromChanged);
	}

	ZMath::mat4 UPointLightComponnet::GetViewProject()
	{
		return m_ViewProject;
	}

	void UPointLightComponnet::OnTransfromChanged(UTransformComponent* TransformComponent)
	{
		ZMath::vec3 LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_View = ZMath::lookAtLH(m_TransformCompent->m_Position, LookAt, ZMath::vec3(0.0f, 1.0f, 0.0f));
		m_ViewProject = m_Projection * m_View;
	}
}
