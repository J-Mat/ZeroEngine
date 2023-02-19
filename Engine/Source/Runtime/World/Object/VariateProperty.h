#pragma once
#include "Core.h"
#include "PropertyObject.h"


namespace Zero
{
	class UVariableProperty : public UProperty
	{
	public:
		UVariableProperty(std::string ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType);
	};
}