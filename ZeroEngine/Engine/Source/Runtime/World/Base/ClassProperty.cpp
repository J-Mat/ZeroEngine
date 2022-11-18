#include "ClassProperty.h"

namespace Zero
{
	UClassProperty::UClassProperty(std::string ClassName, std::string PropertyName, void* Data, const std::string PropertyType, uint32_t DataSize)
		: UProperty(ClassName, PropertyName, Data, PropertyType, DataSize)
	{
	}
}
