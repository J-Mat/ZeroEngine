#include "FieldObject.h"
#include "PropertyObject.h"

namespace Zero
{
	UProperty::UProperty(const std::string& ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType)
		: UField()
		, m_BelongClassName(ClassName)
		, m_PropertyName(PropertyName)
		, m_Data(Data)
		, m_DataSize(DataSize)
		, m_PropertyType(PropertyType)
	{
	}
}
