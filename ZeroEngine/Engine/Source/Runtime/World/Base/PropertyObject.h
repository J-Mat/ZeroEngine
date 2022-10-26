#pragma once
#include "Core.h"
#include "FieldObject.h"

namespace Zero
{
	class UProperty : public UField
	{
		using Supper = UField;
	public: 
		UProperty(const std::string& ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType);

		template<class T>
		T* GetData() const
		{
			return (T*)m_Data;
		}
		
		uint32_t GetDataSize() { return m_DataSize; }
		void* GetData() const { return m_Data; }
		const std::string& GetPropertyType() const { return m_PropertyType; }
		const char* GetPropertyName(bool bTrimString = true) const 
		{ 
			if (bTrimString)
			{
				if (m_PropertyName.starts_with("m_"))
				{
					return m_PropertyName.c_str() + 2;
				}
			}
			return m_PropertyName.c_str();
		}

		void InitializeValue(void* InDest) { m_Data = (char*)InDest; }
		void SetDataSize(uint32_t InSize) {  m_DataSize = InSize; }
		const std::string& GetBelongClassName() { return m_BelongClassName; }
	protected:
		void* m_Data = nullptr;
		uint32_t m_DataSize = 0;
		const std::string m_BelongClassName;
		std::string m_PropertyType;
		std::string m_PropertyName;
	};
}