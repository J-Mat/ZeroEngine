#include "ArrayPropretyObject.h"

namespace Zero
{
	UArrayProperty::UArrayProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string ValueType, uint32_t InValueSize)
		:UContainerProperty(ClassName, PropertyName, Data, PropertyType, PropertySize, ValueType, InValueSize)
	{
	}
}
