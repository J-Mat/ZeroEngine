#include "ClassProperty.h"

namespace Zero
{
	UClassProperty::UClassProperty(std::string ClassName, void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(ClassName, PropertyType, Data, DataSize, PropertyType)
	{
		g_AllUObjects.insert(PropertyType);
	}
}
