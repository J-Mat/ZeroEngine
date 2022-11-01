#include "DirectLightComponent.h"

namespace Zero
{
	void UDirectLightComponnet::PostInit()
	{
		ULightComponent::PostInit();
		m_TransformCompent->GetTransformChange().AddFunction(this, &UDirectLightComponnet::OnTransfromChanged);
	}

	ZMath::mat4 UDirectLightComponnet::GetViewProject()
	{
		return m_ViewProject;
	}

	void UDirectLightComponnet::OnTransfromChanged(UTransformComponent* TransformComponent)
	{
		ZMath::vec3 LookAt = m_TransformCompent->m_Position + m_TransformCompent->m_ForwardVector;
		m_View = ZMath::lookAtLH(m_TransformCompent->m_Position, LookAt, ZMath::vec3(0.0f, 1.0f, 0.0f));
		m_ViewProject = m_Projection * m_View;
	}
}
