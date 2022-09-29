#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	enum class EComponentType
	{
		None = 0,
		TransformationComponent,
		TagComponent,
		MeshVertexComponent,
		MeshRenderComponent,
		CameraComponent
	};

#define COMPONENT_CLASS_TYPE(Type) static  EComponentType GetStaticType() {return EComponentType::##Type;}\
								virtual EComponentType GetEventType() const override {return GetStaticType();}
	class UActor;
	class UComponent : public UCoreObject
	{
	public:
	template<class T, typename ...ParamTypes>
	static T* CreateComponent(UActor* Actor, ParamTypes &&...Params)
	{
		T* Component = new T(Params...);
		Component->SetWorld(Actor->GetWorld());
		Component->PostInit();
		Actor->AddComponent(Component);
		return Component;
	}


	public:
		UComponent()
			: UCoreObject()
		{
		}
		virtual EComponentType GetEventType() const = 0;
	};
}