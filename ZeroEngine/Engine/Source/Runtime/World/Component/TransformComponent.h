#pragma once
#include "../Base/ObjectMacros.h"
#include "Component.h"
#include "TransformComponent.reflection.h"

namespace Zero
{
	UCLASS()
	class UTransformComponent : public UComponent
	{	
		GENERATED_BODY()
	public:
		UTransformComponent();
		virtual void MoveLocal(const ZMath::vec3& Offset);
		virtual void RotateLocal(const ZMath::FEulerAngle& Offset);
		virtual void SetPosition(const ZMath::vec3& Position) { m_Position = Position; };
		virtual void SetRotation(const ZMath::vec3& Rotation);
		virtual void SetScale(const ZMath::vec3& Scale) { m_Scale = Scale; };
		ZMath::quat GetOrientation();
		ZMath::mat4 GetRotationMatrix() const;
		ZMath::mat4 GetTransform();

		ZMath::vec3 GetPosition() const { return m_Position; }
		ZMath::vec3 GetRotation() const { return  m_Rotation; }
		ZMath::vec3 GetScale() const { return m_Scale; }

		ZMath::vec3& GetForwardVector() { return m_ForwardVector; }
		ZMath::vec3& GetRightVector() { return m_RightVector; }
		ZMath::vec3& GetUPVector() { return m_UpVector; }
	public:
		UPROPERTY()
		ZMath::vec3 m_Position = {0.0f, 0.0f, 0.0f};

		UPROPERTY()
		ZMath::FRotation m_Rotation = { 0.0f, 0.0f, 0.0f };

		UPROPERTY()
		ZMath::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };

		ZMath::vec3 m_ForwardVector = { 0.0f, 0.0f, 1.0f };
		ZMath::vec3 m_RightVector = { 1.0f, 0.0f, 0.0f };
		ZMath::vec3 m_UpVector = { 0.0f, 1.0f, 0.0f };
	};
}
