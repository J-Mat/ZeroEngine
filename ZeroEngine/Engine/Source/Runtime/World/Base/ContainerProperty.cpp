#include "ContainerProperty.h"

namespace Zero
{
	UContainerProperty::UContainerProperty(std::string ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(ClassName, PropertyName, Data, DataSize, PropertyType)
	{
	}

	void* UContainerProperty::AllocateData()
	{
		return nullptr;
	}
}
