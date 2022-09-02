#include "Actor.h"
#include "World/Base/CoreObject.h"
#include "World/Component/TransformComponent.h"

namespace Zero
{
	UActor::UActor()
	{
		TransformationComponent = CreateObject<UTransformationComponent>(new UTransformationComponent());
	}
	void UActor::SetPosition(const ZMath::vec3& Position)
	{
		TransformationComponent->SetPosition(Position);
	}
	void UActor::SetRotation(const ZMath::vec3& Rotation)
	{
		TransformationComponent->SetRotation(Rotation);
	}
	void UActor::SetScale(const ZMath::vec3& Scale)
	{
		TransformationComponent->SetScale(Scale);
	}
	ZMath::quat UActor::GetOrientation()
	{
		return TransformationComponent->GetOrientation();
	}
	ZMath::mat4 UActor::GetTransform()
	{
		return TransformationComponent->GetTransform();
	}
	ZMath::vec3 UActor::GetPosition()
	{
		return TransformationComponent->GetPosition();
	}
	ZMath::vec3 UActor::GetRotation()
	{
		return TransformationComponent->GetRotation();
	}
	ZMath::vec3 UActor::GetScale()
	{
		return TransformationComponent->GetScale();
	}
	ZMath::vec3& UActor::GetForwardVector()
	{
		return TransformationComponent->GetForwardVector();
	}
	ZMath::vec3& UActor::GetRightVector()
	{
		return TransformationComponent->GetRightVector();
	}
	ZMath::vec3& UActor::GetUPVector()
	{
		return TransformationComponent->GetUPVector();
	}
}
