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
		m_CameraComponent = CreateObject<UCameraComponent>(GetWorld());
		AddComponent(m_CameraComponent);
	}

	void UCameraActor::Tick()
	{
		
	}

}
