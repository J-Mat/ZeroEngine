#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	class UTransformationComponent;
	class UTagComponent;
	class UActor : public UCoreObject
	{
	public:
		UActor(const std::string Tag = "Actor");
		UTransformationComponent* GetTransformationComponent() { return TransformationComponent; }
		virtual void SetPosition(const ZMath::vec3& Position);
		virtual void SetRotation(const ZMath::vec3& Rotation);
		virtual void SetScale(const ZMath::vec3& Scale);
		ZMath::quat GetOrientation();
		ZMath::mat4 GetTransform();

		ZMath::vec3 GetPosition();
		ZMath::vec3 GetRotation();
		ZMath::vec3 GetScale();

		ZMath::vec3& GetForwardVector();
		ZMath::vec3& GetRightVector();
		ZMath::vec3& GetUPVector();
	protected:
		UTransformationComponent* m_TransformationComponent;
		UTagComponent* m_Tagcomponent;
	};
}