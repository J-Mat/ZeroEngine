#pragma once

namespace Zero
{
	class UCoreObject;

	template<class T>
	T* CreateObject(UCoreObject* NewObject)
	{
		return dynamic_cast<T*>(NewObject);
	}

	template<class T, typename ...ParamTypes>
	T* ConstructionObject(ParamTypes &&...Params)
	{
		return CreateObject<T>(new T(Params...));
	}
}