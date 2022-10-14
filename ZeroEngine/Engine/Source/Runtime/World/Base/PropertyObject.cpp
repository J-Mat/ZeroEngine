#include "FieldObject.h"
#include "PropertyObject.h"

namespace Zero
{
	UProperty::UProperty(void* Data, uint32_t DataSize, EPropertyType PropertyType)
		: UField()
		, m_Data(Data)
		, m_DataSize(DataSize)
		, m_PropertyType(PropertyType)
	{
	}
}
