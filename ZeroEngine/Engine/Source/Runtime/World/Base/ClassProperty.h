#pragma once
#include "Core.h"
#include "PropertyObject.h"

namespace Zero
{
	class UClassProperty : public UProperty
	{
	public:
		UClassProperty(std::string ClassName,  std::string PropertyName, void* Data, const std::string PropertyType, uint32_t DataSize);
	};
}