#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	class UTransformationComponent;
	class UTagComponent;
	class UComponent;
	class UWorld;
	class UActor : public UCoreObject
	{
	public:
		template<class T, typename ...ParamTypes>
		static T* Create(UWorld *World, ParamTypes &&...Params)
		{
			T* Obj = new T(std::forward<ParamTypes>(Params)...);
			Obj->SetWorld(World);
			Obj->PostInit();
			World->AddActor(Obj);
			return Obj;
		}

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

		UActor(const std::string Tag = "Actor");
		virtual void PostInit();
		UTransformationComponent* GetTransformationComponent() { return m_TransformationComponent; }
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
		UTransformationComponent* m_TransformationComponent = nullptr;
		UTagComponent* m_Tagcomponent = nullptr;
		std::vector<UComponent*> m_Components;
	};
}