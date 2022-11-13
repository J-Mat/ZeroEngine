#include "ContainerPropertyObject.h"

namespace Zero
{
	UContainerProperty::UContainerProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string ValueType, uint32_t InValueSize)
		: UProperty(ClassName, PropertyName, Data, PropertySize, PropertyType)
	{
	}

	bool UContainerProperty::AddProterty(void* Data, const std::string& Type, uint32_t ValueSize)
	{
		uint32_t PropertyNum = GetClassCollection().GetPropertyNum();	
		
		std::string ObjectName = std::format("{0}", PropertyNum);
		
		return GetClassCollection().AddVariableProperty("Container", ObjectName, Data, Type, ValueSize) != nullptr;
	}

	bool UContainerProperty::RemoveTailProperty()
	{
		return m_ClassInfoCollection.RemoveTailProperty();
	}

	void UContainerProperty::RemoveAllPreperties()
	{
		m_ClassInfoCollection.RemoveAllProperties();
	}

	void UContainerProperty::UpdateProperty()
	{
		uint32_t Index = 0;
		
		UProperty* Property = GetClassCollection().HeadProperty;
		while (Property)
		{
			void* Ptr = &m_Data[Index * m_ValueSize];
			Property->InitializeValue((char*)Ptr);
			UProperty* NextProperty = static_cast<UProperty*>(Property->Next);
			++Index;
		}
	}

	bool UContainerProperty::UpdateEditorPropertyDetails(UProperty* Property)
	{
		return false;
	}

	void* UContainerProperty::AllocateData()
	{
		uint32_t CurrentIndex = m_ValueSize * m_ValueCount;
		
		if (!m_Data)
		{
			m_Data = (char*)malloc(m_ValueSize);
		}
		else
		{
			m_Data = (char*)realloc(m_Data, m_ValueSize * m_ValueCount + m_ValueSize);
		}
		
		memset(&m_Data[CurrentIndex], 0, m_ValueSize);

		m_ValueCount++;
		
		return &m_Data[CurrentIndex];
	}
	bool UContainerProperty::RemoveTopData()
	{
		if (m_ValueCount > 0)
		{
			--m_ValueCount;
			if (m_ValueCount == 0)
			{
				free(m_Data);
				m_Data = nullptr;
			}
			else
			{
				m_Data = (char*)realloc(m_Data, m_ValueSize * m_ValueCount);
			}
			return true;
		}
		
		return false;
	}
	bool UContainerProperty::RemoveAllData()
	{
		if (m_ValueCount > 0)
		{
			free(m_Data);
			m_Data = nullptr;
			m_ValueCount = 0;
			
			return true;
		}
		return false;
	}
}
