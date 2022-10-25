#include "VariateProperty.h"

namespace Zero
{
	UVariableProperty::UVariableProperty(std::string ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType)
		: UProperty(ClassName, PropertyName, Data, DataSize, PropertyType)
	{
		if (PropertyType == "int" || PropertyType == "int32_t")
		{
			m_VariableType = EPropertyType::PT_Int32;
		}
		else if (PropertyType == "uint32_t")
		{
			m_VariableType = EPropertyType::PT_UInt32;
		}
		else if (PropertyType == "float")
		{
			m_VariableType = EPropertyType::PT_Float;
		}
		else if (PropertyType == "double")
		{
			m_VariableType = EPropertyType::PT_Double;
		}
		else if (PropertyType.ends_with("vec3"))
		{
			m_VariableType = EPropertyType::PT_Vec3;
		}
		else if (PropertyType.ends_with("vec4"))
		{
			m_VariableType = EPropertyType::PT_Vec4;
		}
		else if (PropertyType.ends_with("Color"))
		{
			m_VariableType = EPropertyType::PT_Color;
		}
		else if (PropertyType.ends_with("std::vector"))
		{
			m_VariableType = EPropertyType::PT_Vector;
		}
		else if (PropertyType.ends_with("std::map"))
		{
			m_VariableType= EPropertyType::PT_Map;
		}
		
	}
}
