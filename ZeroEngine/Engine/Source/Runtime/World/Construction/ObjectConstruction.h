#pragma once
#include "World/World.h"

namespace Zero
{
	class UCoreObject;

	template<class T, typename ...ParamTypes>
	T* CreateObject(UWorld *World, ParamTypes &&...Params)
	{
		T* Obj = new T(Params...));
		Obj->SetWorld(World);
		return Obj;
	}
}