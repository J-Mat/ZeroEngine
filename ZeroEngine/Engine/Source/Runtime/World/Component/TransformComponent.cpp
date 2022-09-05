#include "TransformComponent.h"
#include "ZMath.h"

namespace Zero
{
	UTransformationComponent::UTransformationComponent()
		: UComponent()
	{
	}
	void UTransformationComponent::SetRotation(const ZMath::vec3& Rotation)
	{
		m_Rotation = {
			ZMath::radians(Rotation.x),
			ZMath::radians(Rotation.y),
			ZMath::radians(Rotation.z),
		};
		ZMath::quat Quat = GetOrientation();
		m_UpVector = ZMath::rotate(Quat, ZMath::World_Up);
		m_ForwardVector = ZMath::rotate(Quat, ZMath::World_Forward);
		m_RightVector = ZMath::rotate(Quat, ZMath::World_Right);
	}
	ZMath::quat UTransformationComponent::GetOrientation()
	{
		return ZMath::quat(m_Rotation);
	}
	ZMath::mat4 UTransformationComponent::GetTransform()
	{
		return ZMath::translate(ZMath::mat4(1.0f), m_Position) * ZMath::toMat4(GetOrientation()) * ZMath::scale(ZMath::mat4(1.0f), m_Scale);
	}
}
