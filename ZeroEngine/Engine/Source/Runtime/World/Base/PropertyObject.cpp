#include "FieldObject.h"
#include "PropertyObject.h"

namespace Zero
{
	UProperty::UProperty(void* Data, uint32_t DataSize, const std::string PropertyType)
		: UField()
		, m_Data(Data)
		, m_DataSize(DataSize)
	{
		if (PropertyType == "int")
		{
			m_PropertyType = PT_Int;
		}
		else if (PropertyType == "float")
		{
			m_PropertyType = PT_Float;
		}
		else if (PropertyType == "double")
		{
			m_PropertyType = PT_Double;
		}
		else if (PropertyType.ends_with("vec3"))
		{
			m_PropertyType = PT_Vec3;
		}
		else if (PropertyType.ends_with("vec4"))
		{
			m_PropertyType = PT_Vec4;
		}
		else if (PropertyType.ends_with("Color"))
		{
			m_PropertyType = PT_Color;
		}
		else if (PropertyType.find("vector") != std::string::npos)
		{
			m_PropertyType = PT_Vector;
		}
		else if (PropertyType.find("map") != std::string::npos)
		{
			m_PropertyType = PT_Map;
		}
	}
}
