#include "TransformComponent.h"
#include "Math/ZMath.h"

namespace Zero
{
	UTransformationComponent::UTransformationComponent()
		: UComponent()
	{
	}

	void UTransformationComponent::MoveLocal(const ZMath::vec3& Offset)
	{
		m_Position += m_RightVector * Offset.x + m_UpVector * Offset.y + m_ForwardVector * Offset.z;
	}

	void UTransformationComponent::RotateLocal(const ZMath::FEulerAngle& Offset)
	{
		ZMath::vec3 Rotate = { m_Rotation.x + Offset.Pitch, m_Rotation.y + Offset.Yaw, m_Rotation.z + Offset.Roll };
		SetRotation(Rotate);
	}

	void UTransformationComponent::SetRotation(const ZMath::vec3& Rotation)
	{
		m_Rotation = {
			ZMath::radians(Rotation.x),
			ZMath::radians(Rotation.y),
			ZMath::radians(Rotation.z),
		};
		
		m_ForwardVector.x = ZMath::cos(m_Rotation.x) * ZMath::sin(m_Rotation.y);
		m_ForwardVector.y = ZMath::sin(m_Rotation.x);
		m_ForwardVector.z = ZMath::cos(m_Rotation.x) * ZMath::cos(m_Rotation.y);

		
		m_RightVector.x = ZMath::cos(m_Rotation.z) * ZMath::cos(-m_Rotation.y);
		m_RightVector.y = ZMath::sin(m_Rotation.z);
		m_RightVector.z = ZMath::cos(m_Rotation.z) * ZMath::sin(-m_Rotation.y);
		
		m_UpVector = ZMath::cross(m_ForwardVector, m_RightVector);
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
