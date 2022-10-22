#include "ClassProperty.h"

namespace Zero
{
	UClassProperty::UClassProperty(void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(Data, DataSize, PropertyType)
	{
		g_AllUObjects.insert(PropertyType);
	}
}
