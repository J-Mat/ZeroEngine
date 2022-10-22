#include "FieldObject.h"
#include "PropertyObject.h"

namespace Zero
{
	UProperty::UProperty(void* Data, uint32_t DataSize, const std::string PropertyType)
		: UField()
		, m_Data(Data)
		, m_DataSize(DataSize)
	{
	}
}
