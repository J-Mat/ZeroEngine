#include "ObjectGlobal.h"

namespace Zero
{
	std::map<Utils::Guid, UCoreObject*>& FObjectGlobal::GetObjectCollection()
	{
		static std::map<Utils::Guid, UCoreObject*> g_ObjectCollection;
		return g_ObjectCollection;
	}

	std::map<std::string, FClassID>& FObjectGlobal::GetClassInfoMap()
	{
		static std::map<std::string, FClassID> g_ClassInfo;
		return g_ClassInfo;
	}

	std::map<std::string, FEnumElement>& FObjectGlobal::GetRegiterEnumMap()
	{
		static std::map<std::string, FEnumElement> g_AllEnumNames;
		return g_AllEnumNames;
	}

	void FObjectGlobal::RegisterEnumObject(const std::string& EnumName, const FEnumElement& EnumElement)
	{
		GetRegiterEnumMap().insert({EnumName, EnumElement});
	}
}
