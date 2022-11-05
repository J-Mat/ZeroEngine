#include "TransformComponent.h"
#include "TransformComponent.h"
#include "Math/ZMath.h"

namespace Zero
{
	UTransformComponent::UTransformComponent()
		: UComponent()
	{
	}

	void UTransformComponent::MoveLocal(const ZMath::vec3& Offset)
	{
		m_Position += m_RightVector * Offset.x + m_UpVector * Offset.y + m_ForwardVector * Offset.z;
	}

	void UTransformComponent::RotateLocal(const ZMath::FEulerAngle& Offset)
	{
		ZMath::vec3 Rotate = { m_Rotation.x + Offset.Pitch, m_Rotation.y + Offset.Yaw, m_Rotation.z + Offset.Roll };
		SetRotation(Rotate);
	}

	void UTransformComponent::SetPosition(const ZMath::vec3& Position)
	{
		m_Position = Position;
		m_OnTransformChanged.Broadcast(this);
	}

	void UTransformComponent::PostInit()
	{
	}

	void UTransformComponent::UpdateDirection()
	{
		//m_ForwardVector =  ZMath::normalize(ZMath::eulerAngles(ZMath::quat(m_Rotation)));
		m_ForwardVector.x = ZMath::cos(m_Rotation.x) * ZMath::sin(m_Rotation.y);
		m_ForwardVector.y = ZMath::sin(m_Rotation.x);
		m_ForwardVector.z = ZMath::cos(m_Rotation.x) * ZMath::cos(m_Rotation.y);

		m_RightVector.x = ZMath::cos(m_Rotation.z) * ZMath::cos(-m_Rotation.y);
		m_RightVector.y = ZMath::sin(m_Rotation.z);
		m_RightVector.z = ZMath::cos(m_Rotation.z) * ZMath::sin(-m_Rotation.y);
		
		m_UpVector = ZMath::cross(m_ForwardVector, m_RightVector);


		
		ZMath::mat4 mat =	glm::toMat4(glm::quat(m_Rotation));

		m_ForwardVector = mat[2];
		m_RightVector = mat[0];
		m_UpVector = mat[1];
		
		
		//std::cout << m_ForwardVector << "-----" << ZMath::vec3(test) << std::endl;
	}

	void UTransformComponent::SetRotation(const ZMath::vec3& Rotation)
	{
		m_Rotation = {
			ZMath::radians(Rotation.x),
			ZMath::radians(Rotation.y),
			ZMath::radians(Rotation.z),
		};
	
		
		UpdateDirection();

		m_OnTransformChanged.Broadcast(this);
	}

	void UTransformComponent::SetScale(const ZMath::vec3& Scale)
	{
		m_Scale = Scale;
		m_OnTransformChanged.Broadcast(this);
	}

	void UTransformComponent::SetTransfrom(ZMath::mat4& Transform)
	{
		ZMath::vec3 Translation, Rotation,	Scale;
		ZMath::DecomposeTransform(Transform, Translation, Rotation, Scale);
		m_Position = Translation;
		m_Rotation = Rotation;	
		m_Scale = Scale;
		UpdateDirection();	
		m_OnTransformChanged.Broadcast(this);
	}

	ZMath::quat UTransformComponent::GetOrientation()
	{
		return ZMath::quat(m_Rotation);
	}

	ZMath::mat4 UTransformComponent::GetRotationMatrix() const
	{
		ZMath::mat4 mat =  glm::toMat4(glm::quat(m_Rotation));
		return mat;
	}

	ZMath::mat4 UTransformComponent::GetTransform()
	{
		return ZMath::translate(ZMath::mat4(1.0f), m_Position) * GetRotationMatrix()* ZMath::scale(ZMath::mat4(1.0f), m_Scale);
	}
	void UTransformComponent::PostEdit(UProperty* Property)
	{
		if (Property->GetPropertyName() == "m_Rotation")
		{
			UpdateDirection();
		}
		m_OnTransformChanged.Broadcast(this);
	}
}
