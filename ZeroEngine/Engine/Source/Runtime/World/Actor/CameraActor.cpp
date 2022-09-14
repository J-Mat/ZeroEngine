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

}
