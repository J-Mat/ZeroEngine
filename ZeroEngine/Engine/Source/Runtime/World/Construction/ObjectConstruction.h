#pragma once
namespace Zero
{
	class UCoreObject;

	class UWorld;
	template<class T, typename ...ParamTypes>
	T* CreateObject(UWorld *World, ParamTypes &&...Params)
	{
		T* Obj = new T(Params...);
		//Obj->SetWorld(World);
		Obj->PostInit();
		return Obj;
	}
}