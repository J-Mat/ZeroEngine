#include "Actor.h"
#include "World/Base/CoreObject.h"
#include "World/Component/TransformComponent.h"
#include "World/Component/TagComponent.h"

namespace Zero
{
	UActor::UActor()
		: UCoreObject()
	{
	}
	void UActor::PostInit()
	{
	//	m_TransformationComponent = UComponent::CreateComponent<UTransformationComponent>(this);
		//m_Tagcomponent = UComponent::CreateComponent<UTagComponent>(this, m_Tag);
	}

	void UActor::MoveLocal(const ZMath::vec3& Offset)
	{
		m_TransformationComponent->MoveLocal(Offset);
	}
	void  UActor::RotateLocal(const ZMath::FEulerAngle& Offset)
	{
		m_TransformationComponent->RotateLocal(Offset);
	}

	void UActor::SetPosition(const ZMath::vec3& Position)
	{
		m_TransformationComponent->SetPosition(Position);
	}
	void UActor::SetRotation(const ZMath::vec3& Rotation)
	{
		m_TransformationComponent->SetRotation(Rotation);
	}
	void UActor::SetScale(const ZMath::vec3& Scale)
	{
		m_TransformationComponent->SetScale(Scale);
	}
	void UActor::Tick()
	{
		for (UComponent* Component : m_Components)
		{
			Component->Tick();
		}
	}
	ZMath::quat UActor::GetOrientation()
	{
		return m_TransformationComponent->GetOrientation();
	}
	ZMath::mat4 UActor::GetTransform()
	{
		return m_TransformationComponent->GetTransform();
	}
	ZMath::vec3 UActor::GetPosition()
	{
		return m_TransformationComponent->GetPosition();
	}
	ZMath::vec3 UActor::GetRotation()
	{
		return m_TransformationComponent->GetRotation();
	}
	ZMath::vec3 UActor::GetScale()
	{
		return m_TransformationComponent->GetScale();
	}
	ZMath::vec3& UActor::GetForwardVector()
	{
		return m_TransformationComponent->GetForwardVector();
	}
	ZMath::vec3& UActor::GetRightVector()
	{
		return m_TransformationComponent->GetRightVector();
	}
	ZMath::vec3& UActor::GetUPVector()
	{
		return m_TransformationComponent->GetUPVector();
	}
}
