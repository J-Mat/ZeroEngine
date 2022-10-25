#pragma once
#include "Core.h"
#include "PropertyObject.h"

namespace Zero
{
	class UClassProperty : public UProperty
	{
	public:
		UClassProperty(std::string ClassName, void* Data, uint32_t DataSize, const std::string PropertyType);
	};
}