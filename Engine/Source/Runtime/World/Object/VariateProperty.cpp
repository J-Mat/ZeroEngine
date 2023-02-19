#include "VariateProperty.h"

namespace Zero
{
	UVariableProperty::UVariableProperty(std::string ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(ClassName, PropertyName, Data, PropertyType, DataSize)
	{
	}
}
