#pragma once
#include "Core.h"
#include "PropertyObject.h"


namespace Zero
{
	enum EPropertyType
	{
		PT_Unknown,
		PT_UInt32,
		PT_Int32,
		PT_Float,
		PT_Double,
		PT_Vec3,
		PT_Vec4,
		PT_Color,
		PT_Vector,
		PT_Map,
	};
	class UVariableProperty : public UProperty
	{
	public:
		UVariableProperty(std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType);
	private:
		EPropertyType m_VariableType = PT_Unknown;
	};
}