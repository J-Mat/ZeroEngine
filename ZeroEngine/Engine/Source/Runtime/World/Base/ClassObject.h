#pragma once
#include "FieldObject.h"
#include "Delegate.h"

namespace Zero
{	
	class UClass;
	struct FClassID
	{
		FClassID() = default;
		FClassID(std::string _ClassName, UClass* _Class)
			: DerivedClassName(_ClassName)
			, Class(_Class)
		{}
		std::string DerivedClassName = "";
		UClass* Class = nullptr;
	};
	DEFINITION_SIMPLE_SINGLE_DELEGATE(FRegisterClassObjectDelegate, UCoreObject*);
	class UClass : public UField
	{
	public:
		using Supper = UField;
		UClass();
	
		UCoreObject* GetDefaultObject();

		FRegisterClassObjectDelegate m_RegisterClassObjectDelegate;
	private:
		UCoreObject* m_DefaultObject = nullptr;
	};
}