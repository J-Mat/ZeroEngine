#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"
#include "../Base/ObjectGenerator.h"

namespace Zero
{
	class UTransformComponent;
	class UTagComponent;
	class UComponent;
	class UWorld;
	UCLASS()
	class UActor : public UCoreObject
	{
	public:
		UActor();

		UTransformComponent* m_RootComponent;

		UTransformComponent* GetRootComponent() const { return m_RootComponent; }


		template<class T>
		T* GetComponent()
		{
			for (UComponent* Component : m_Components)
			{
				if (Component->GetEventType() == T::GetStaticType())
				{
					return static_cast<T*>(Component);
				}
			}
			return nullptr;
		}

		template<>
		UTransformComponent* GetComponent()
		{
			return m_TransformationComponent;
		}

		virtual void PostInit();
		virtual void MoveLocal(const ZMath::vec3& Offset);
		virtual void RotateLocal(const ZMath::FEulerAngle& Offset);
		virtual void SetPosition(const ZMath::vec3& Position);
		virtual void SetRotation(const ZMath::vec3& Rotation);
		virtual void SetScale(const ZMath::vec3& Scale);
		virtual ZMath::FAABB GetAABB() { return ZMath::FAABB(); }
		virtual void Tick();
		ZMath::quat GetOrientation();
		ZMath::mat4 GetTransform();

		ZMath::vec3 GetPosition();
		ZMath::vec3 GetRotation();
		ZMath::vec3 GetScale();

		ZMath::vec3& GetForwardVector();
		ZMath::vec3& GetRightVector();
		ZMath::vec3& GetUPVector();
		std::vector<UComponent*>& GetAllComponents() { return m_Components; }
		void AddComponent(UComponent* Component) { m_Components.push_back(Component); }
		std::string GetTag() { return m_Tag; }
	protected:
		std::string m_Tag;
		UTransformComponent* m_TransformationComponent = nullptr;
		UTagComponent* m_Tagcomponent = nullptr;
		std::vector<UComponent*> m_Components;
	};
}