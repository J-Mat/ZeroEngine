#include "ArrayPropretyObject.h"
#include "MapPropretyObject.h"

namespace Zero
{
	Zero::UMapProperty::UMapProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string KeyType, uint32_t KeySize, std::string ValueType, uint32_t InValueSize)
		:UContainerProperty(ClassName, PropertyName, Data, PropertyType, PropertySize)
		, m_ValueType(ValueType)
		, m_ValueSize(InValueSize)
		, m_KeyType(KeyType)
		, m_KeySize(KeySize)
	{
	}

	UProperty* UMapProperty::AddProperty(void* Data, const std::string& Type, uint32_t ValueSize)
	{
		uint32_t PropertyNum = GetClassCollection().GetPropertyNum() / 2;	
		
		std::string ObjectName = std::format("{0}", PropertyNum);
		
		return (UProperty*)GetClassCollection().AddVariableProperty("Container", ObjectName, Data, Type, ValueSize);
	}

	void UMapProperty::UpdateProperty()
	{
		uint32_t CurrentIndex = 0;
		UProperty* KeyProperty = GetClassCollection().HeadProperty;
		while (KeyProperty)
		{
			void* KeyPtr = &m_Data[CurrentIndex];
			CurrentIndex += m_KeySize;
			KeyProperty->InitializeValue(KeyPtr);

			UProperty* ValueProperty = static_cast<UProperty*>(KeyProperty->Next);
			void* ValuePtr = &m_Data[CurrentIndex];
			CurrentIndex += m_ValueSize;
			ValueProperty->InitializeValue(ValuePtr);

			KeyProperty = static_cast<UProperty*>(ValueProperty->Next);
		}
	}

	UProperty* UMapProperty::AddItem()
	{
		void* KeyData = AllocateData(m_KeySize, m_KeyType);
		UProperty* KeyProperty =  AddProperty(KeyData, m_KeyType, m_KeySize);

		void* ValueData = AllocateData(m_ValueSize, m_ValueType);
		UProperty* ValueProperty = AddProperty(ValueData, m_ValueType, m_ValueSize);

		UpdateProperty();
		return KeyProperty;
	}

	bool UMapProperty::RemoveTailItem()
	{
		if (m_WholeDataSize > 0)
		{
			// Value
			RemoveTailProperty();
			RemoveTailData(m_ValueSize);

			// Key
			RemoveTailProperty();
			RemoveTailData(m_KeySize);

			UpdateProperty();
			return true;
		}
		return false;
	}

	bool UMapProperty::RemoveAllItem()
	{
		if (m_WholeDataSize > 0)
		{
			RemoveAllProperties();
			RemoveAllData();
			return true;
		}
		return false;
	}
}
