#include "ContainerPropertyObject.h"

namespace Zero
{
	UContainerProperty::UContainerProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize)
		: UProperty(ClassName, PropertyName, Data, PropertyType, PropertySize)
	{
	}

	bool UContainerProperty::RemoveTailProperty()
	{
		return m_ClassInfoCollection.RemoveTailProperty();
	}

	void UContainerProperty::RemoveAllPreperties()
	{
		m_ClassInfoCollection.RemoveAllProperties();
	}

	void* UContainerProperty::AllocateData(uint32_t ValueSize, const std::string& ValueType)
	{
		uint32_t CurrentIndex = m_WholeDataSize;
		
		if (!m_Data)
		{
			m_Data = (char*)malloc(ValueSize);
		}
		else
		{
			m_Data = (char*)realloc(m_Data, m_WholeDataSize + ValueSize);
		}
		
		memset(&m_Data[CurrentIndex], 0, ValueSize);

		if (ValueType == "std::string") 
		{
			new (&m_Data[CurrentIndex])std::string();
		}
		else if (ValueType == "FTextureHandle")
		{
			new (&m_Data[CurrentIndex])FTextureHandle();
		}
		else if (ValueType == "FFloatSlider")
		{
			new (&m_Data[CurrentIndex])FFloatSlider();
		}

		m_WholeDataSize += ValueSize;
		
		return &m_Data[CurrentIndex];
	}


	void UContainerProperty::RemoveTailData(uint32_t ValueSize)
	{
		CORE_ASSERT(m_WholeDataSize >= ValueSize, "Ivalid Memory Free!");
		m_WholeDataSize -= ValueSize;
		if (m_WholeDataSize == 0)
		{
			free(m_Data);
			m_Data = nullptr;
		}
		else
		{
			m_Data = (char*)realloc(m_Data, m_WholeDataSize);
		}
	}

	void UContainerProperty::RemoveAllData()
	{
		if (m_WholeDataSize > 0)
		{
			free(m_Data);
			m_Data = nullptr;
			m_WholeDataSize = 0;
		}
	}
}
