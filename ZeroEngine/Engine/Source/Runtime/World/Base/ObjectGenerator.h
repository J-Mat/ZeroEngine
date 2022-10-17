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
		Obj->SetOuter(Outer);
		Obj->PostInit();
		return Obj;
	}


	template<typename T, typename ... Args>
	T* CreateComponent(UActor* Actor, UComponent* RootComponent, Args&&...Params)
	{
		T* Component = new T(std::forward<Args>(args)...);
		Component->SetOuter(Actor);
		Actor->AddComponent(Component);
		Component->SetParentComponent(RootComponent);
		return Component;
	}
}
