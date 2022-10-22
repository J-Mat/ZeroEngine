#include "VariateProperty.h"

namespace Zero
{
	UVariableProperty::UVariableProperty(void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(Data, DataSize, PropertyType)
	{
		if (PropertyType == "int" || PropertyType == "int32_t")
		{
			m_PropertyType = EPropertyType::PT_Int32;
		}
		else if (PropertyType == "uint32_t")
		{
			m_PropertyType = EPropertyType::PT_UInt32;
		}
		else if (PropertyType == "float")
		{
			m_PropertyType = EPropertyType::PT_Float;
		}
		else if (PropertyType == "double")
		{
			m_PropertyType = EPropertyType::PT_Double;
		}
		else if (PropertyType.ends_with("vec3"))
		{
			m_PropertyType = EPropertyType::PT_Vec3;
		}
		else if (PropertyType.ends_with("vec4"))
		{
			m_PropertyType = EPropertyType::PT_Vec4;
		}
		else if (PropertyType.ends_with("Color"))
		{
			m_PropertyType = EPropertyType::PT_Color;
		}
		else if (PropertyType.ends_with("std::vector"))
		{
			m_PropertyType = EPropertyType::PT_Vector;
		}
		else if (PropertyType.ends_with("std::map"))
		{
			m_PropertyType = EPropertyType::PT_Map;
		}
		
	}
}
