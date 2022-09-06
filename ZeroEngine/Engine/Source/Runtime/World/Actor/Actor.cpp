#include "Actor.h"
#include "World/Base/CoreObject.h"
#include "World/Component/TransformComponent.h"
#include "World/Component/TagComponent.h"
#include "World/Construction/ObjectConstruction.h"

namespace Zero
{
	UActor::UActor(const std::string Tag)
		: UCoreObject()
	{
		m_TransformationComponent = CreateObject<UTransformationComponent>(GetWorld());
		m_Tagcomponent = CreateObject<UTagComponent>(GetWorld(), Tag);
		
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