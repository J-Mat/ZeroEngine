#pragma once

#include "CoreObject.h"
#include "ClassObject.h"

namespace Zero
{
	std::map<Utils::Guid, UCoreObject*>& GetObjectCollection();
	std::map<std::string, FClassID>& GetClassInfoMap();
}