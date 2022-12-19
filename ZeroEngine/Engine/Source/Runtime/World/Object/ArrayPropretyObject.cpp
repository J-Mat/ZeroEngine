#include "ArrayPropretyObject.h"

namespace Zero
{
	UArrayProperty::UArrayProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string ValueType, uint32_t InValueSize)
		:UContainerProperty(ClassName, PropertyName, Data, PropertyType, PropertySize)
		, m_ValueType(ValueType)
		, m_ValueSize(InValueSize)
	{
	}
	UProperty* UArrayProperty::AddItem()
	{
		void* CurrentData = AllocateData(m_ValueSize, m_ValueType);
		UProperty* Property =  AddProperty(CurrentData, m_ValueType, m_ValueSize);
		UpdateProperty();
		return Property;
	}

	UProperty* UArrayProperty::AddProperty(void* Data, const std::string& Type, uint32_t ValueSize)
	{
		uint32_t PropertyNum = GetClassCollection().GetPropertyNum();	
		
		std::string ObjectName = std::format("{0}", PropertyNum);
		
		return (UProperty*)GetClassCollection().AddVariableProperty("Container", ObjectName, Data, Type, ValueSize);
	}

	void UArrayProperty::UpdateProperty()
	{
		uint32_t Index = 0;
		
		UProperty* Property = GetClassCollection().HeadProperty;
		while (Property)
		{
			void* Ptr = &m_Data[Index * m_ValueSize];
			Property->InitializeValue(Ptr);
			Property = static_cast<UProperty*>(Property->Next);
			++Index;
		}
	}

	bool UArrayProperty::RemoveTailItem()
	{
		if (m_WholeDataSize > 0)
		{
			RemoveTailProperty();
			RemoveTailData(m_ValueSize);
			UpdateProperty();
			return true;
		}
		return false;
	}

	bool UArrayProperty::RemoveAllItem()
	{
		if (m_WholeDataSize > 0)
		{
			RemoveAllPreperties();
			RemoveAllData();
			return true;
		}
		return false;
	}
}
