#pragma once
#include "ContainerPropertyObject.h"

namespace Zero
{
	class UArrayProperty : public UContainerProperty
	{
	public:
		UArrayProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string ValueType, uint32_t InValueSize);
	};
}