#include "ObjectGlobal.h"

namespace Zero
{
	std::map<Utils::Guid, UCoreObject*>& GetObjectCollection()
	{
		static std::map<Utils::Guid, UCoreObject*> g_ObjectCollection;
		return g_ObjectCollection;
	}
	std::map<std::string, FClassID>& GetClassInfoMap()
	{
		static std::map<std::string, FClassID> g_ClassInfo;
		return g_ClassInfo;
	}
}
