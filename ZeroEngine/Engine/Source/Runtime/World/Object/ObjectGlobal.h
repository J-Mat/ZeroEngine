#pragma once

#include "CoreObject.h"
#include "ClassObject.h"

namespace Zero
{
	struct FEnumInfo
	{
		int32_t Value;
		std::set<std::string> Fields;
		std::map<std::string, std::string> Metas;
	};
	using FEnumItem = std::pair<std::string, FEnumInfo>;

	struct FEnumElement
	{
		std::string EnumName;
		std::vector<FEnumItem> EnumItemList;
	};

	class FObjectGlobal
	{
	public:
		static std::map<Utils::Guid, UCoreObject*>& GetObjectCollection();
		static std::map<std::string, FClassID>& GetClassInfoMap();
		static std::map<std::string, FEnumElement>& GetRegiterEnumMap();
		static void RegisterEnumObject(const std::string& EnumName, const FEnumElement& EnumElement);
	};
}