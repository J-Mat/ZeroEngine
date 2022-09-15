#include "CameraActor.h"
#include "../Component/CameraComponent.h"
#include "World/Construction/ObjectConstruction.h"

namespace Zero
{
	UCameraActor::UCameraActor(const std::string& Tag)
	{
	}
	void UCameraActor::PostInit()
	{
		m_CameraComponent = UComponent::CreateComponent<UCameraComponent>(this);
		m_TransformationComponent = m_CameraComponent;
	}

	void UCameraActor::Tick()
	{
		UActor::Tick();
	}

	void UCameraActor::SetRotation(const ZMath::vec3& Rotation)
	{
		ZMath::vec3 Tmp = Rotation;
		Tmp.x = ZMath::clamp(Rotation.x, -70.0f, +70.0f);
		Tmp.z = ZMath::clamp(Rotation.z, -89.0f, +89.0f);
		m_TransformationComponent->SetRotation(Tmp);
	}

}
