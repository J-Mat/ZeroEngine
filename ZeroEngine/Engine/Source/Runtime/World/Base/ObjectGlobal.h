#pragma once

#include "CoreObject.h"
#include "ClassObject.h"

namespace Zero
{
	using FEnumValue = std::pair<std::string, int32_t>;

	struct FEnumElement
	{
		std::string EnumName;
		std::vector<FEnumValue> EnumInfoList;
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