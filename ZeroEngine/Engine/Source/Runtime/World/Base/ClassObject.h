#pragma once
#include "FieldObject.h"
#include "Delegate.h"

namespace Zero
{
	class UClass;
	static std::map<std::string, UClass*> g_AllUObjectClasses;
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