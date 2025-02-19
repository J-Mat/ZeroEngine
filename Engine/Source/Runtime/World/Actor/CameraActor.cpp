#include "CameraActor.h"
#include "../Component/CameraComponent.h"

namespace Zero
{
	UCameraActor::UCameraActor()
		: UActor()
	{
		CreateComponent<UCameraComponent>(this);
		m_TransformationComponent->SetPosition({ 0.0f, 0.0f, -3.35f });
	}

	void UCameraActor::PostInit()
	{
		m_CameraComponent = GetComponent<UCameraComponent>();
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

	void UCameraActor::OnResizeViewport(uint32_t Width, uint32_t Height)
	{
		m_CameraComponent->OnResizeViewport(Width, Height);
	}
}
