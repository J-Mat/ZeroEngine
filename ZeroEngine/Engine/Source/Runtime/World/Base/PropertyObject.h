#pragma once
#include "Core.h"
#include "FieldObject.h"

namespace Zero
{


	class UProperty : public UField
	{
		using Supper = UField;
	public: 
		UProperty(void* Data, uint32_t DataSize, const std::string PropertyType);

		template<class T>
		T* GetData() const
		{
			return (T*)m_Data;
		}
		
		uint32_t GetDataSize() { return m_DataSize; }
		void* GetData() const { return m_Data; }

		void InitializeValue(void* InDest) { m_Data = (char*)InDest; }
		void SetDataSize(uint32_t InSize) {  m_DataSize = InSize; }

	protected:
		void* m_Data = nullptr;
		uint32_t m_DataSize = 0;
	};
	
	
}