#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
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
	};
}