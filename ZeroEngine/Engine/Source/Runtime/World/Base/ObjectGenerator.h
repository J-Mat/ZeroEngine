#pragma once

#include "CoreObject.h"
#include "../Actor/Actor.h"
#include "../Component/Component.h"

namespace Zero
{
	template<typename T, typename ... Args>
	T* CreateObject(UCoreObject* Outer, Args&&...Params)
	{
		T* Obj = new T(std::forward<Args>(args)...);
		Obj->SetOuter(Outer);
	}


	template<typename T, typename ... Args>
	T* CreateComonent(UActor* Actor, UComponent* RootComponent, Args&&...Params)
	{
		T* Component = new T(std::forward<Args>(args)...);
		Component->SetOuter(Outer);
		Actor->AddComponent(Component);
		Component->SetParentComponent(RootComponent);
	}
}

