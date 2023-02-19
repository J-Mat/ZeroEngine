#pragma once

//#include "../Actor/Actor.h"
//#include "../Component/Component.h"

namespace Zero
{
	class UWorld;
	class UActor;
	class UComponent;
	class UCoreObject;
	template<typename T, typename ... Args>
	T* CreateObject(UCoreObject* Outer, Args&&...Params)
	{
		T* Obj = new T(std::forward<Args>(Params)...);
		Obj->InitReflectionContent();
		Obj->SetOuter(Outer);
		Obj->PostInit();
		return Obj;
	}

	template<typename T>
	T* CreateObjectRaw(UCoreObject* Outer)
	{
		T* Obj = new T();
		Obj->InitReflectionContent();
		Obj->SetOuter(Outer);
		return Obj;
	}

	template<typename T, typename ... Args>
	T* CreateComponent(UActor* Actor, Args&&...Params)
	{
		T* Component = new T(std::forward<Args>(args)...);
		Component->InitReflectionContent();
		Component->SetOuter(Actor);
		Actor->AddComponent(Component);
		Component->SetParentComponent(Actor->GetRootComponent());
		Component->PostInit();
		return Component;
	}
}
