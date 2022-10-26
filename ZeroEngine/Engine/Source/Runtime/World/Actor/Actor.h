#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"
#include "../Base/ObjectGenerator.h"
#include "Actor.reflection.h"


namespace Zero
{
	class UTransformComponent;
	class UTagComponent;
	class UComponent;
	class UWorld;
	UCLASS()
	class UActor : public UCoreObject
	{
		GENERATED_BODY()
	public:
		static std::map<std::string, uint32_t> s_ObjectNameIndex;
		UActor();

		UComponent* m_RootComponent;

		UComponent* GetRootComponent() const { return m_RootComponent; }
		void SetRootComponent(UComponent* Component) { m_RootComponent = Component; }


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
		void SetWorld(UWorld* World) { m_World = World; }
		UWorld* GetWorld() { return m_World; }
		void SetTagName(const std::string& Name) 
		{ 
			auto Iter = s_ObjectNameIndex.find(Name);
			if (Iter == s_ObjectNameIndex.end())
			{
				s_ObjectNameIndex.insert({Name, 1});
				m_Tag = Name;
			}
			else
			{
				uint32_t Index = Iter->second++;
				m_Tag = std::format("{0}_{1}",Name, Index);
			}
		}
		const std::string& GetTagName() 
		{
			if (m_Tag == "")
			{
				SetTagName(GetObjectName());
			}
			return m_Tag;
		}
	public:
		UPROPERTY()
		std::string m_Tag = "";

		UPROPERTY()
		UTransformComponent* m_TransformationComponent = nullptr;
	protected:
		UWorld* m_World;
		std::vector<UComponent*> m_Components;
	};
}