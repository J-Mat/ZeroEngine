#pragma once

#include "Component.h"


namespace Zero
{
	class UTransformationComponent : public UComponent
	{
	public:
		UTransformationComponent();
		virtual void SetPosition(const ZMath::vec3& Position) { m_Position = Position; };
		virtual void SetRotation(const ZMath::vec3& Rotation);
		virtual void SetScale(const ZMath::vec3& Scale) { m_Scale = Scale; };
		ZMath::quat GetOrientation();
		ZMath::mat4 GetTransform();

		ZMath::vec3 GetPosition() const { return m_Position; }
		ZMath::vec3 GetRotation() const { return  m_Rotation; }
		ZMath::vec3 GetScale() const { return m_Scale; }

		ZMath::vec3& GetForwardVector() { return m_ForwardVector; }
		ZMath::vec3& GetRightVector() { return m_RightVector; }
		ZMath::vec3& GetUPVector() { return m_UpVector; }
	protected:
		ZMath::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		ZMath::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		ZMath::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };

		ZMath::vec3 m_ForwardVector = { 0.0f, 0.0f, 0.0f };
		ZMath::vec3 m_RightVector = { 0.0f, 0.0f, 0.0f };
		ZMath::vec3 m_UpVector = { 0.0f, 0.0f, 0.0f };
	};
}
