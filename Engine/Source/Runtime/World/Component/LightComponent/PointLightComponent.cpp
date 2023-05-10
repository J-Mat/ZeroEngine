#include "PointLightComponent.h"

namespace Zero
{
	void UPointLightComponnet::PostInit()
	{
		ULightComponent::PostInit();
		FOnTransformChanged& OnTransformChanged = m_TransformCompent->GetTransformChange();
		OnTransformChanged.AddFunction(this, &UPointLightComponnet::OnTransfromChanged);
	}

	void UPointLightComponnet::Tick()
	{
		Supper::Tick();
		m_SceneCaptureCube.UpdateParams();
	}

	void UPointLightComponnet::OnTransfromChanged(UTransformComponent* TransformComponent)
	{
		m_SceneCaptureCube.SetViewPos(TransformComponent->GetPosition());
	}
}
