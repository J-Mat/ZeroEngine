#include "ClassProperty.h"

namespace Zero
{
	UClassProperty::UClassProperty(std::string ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(ClassName, PropertyName, Data, DataSize, PropertyType)
	{
	}
}
