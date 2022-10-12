#pragma once
#include "Core.h"
#include "Field.h"

namespace Zero
{
	class UProperty : public UField
	{
		using Supper = UField;
	public: 
		UProperty();

		template<class T>
		T* GetData() const
		{
			return (T*)Data;
		}
		
		uint32_t GetDataSize() { return m_DataSize; }
		void* GetData() const { return m_Data; }
		const std::string& GetType() const { return m_Type; }

		void InitializeValue(void* InDest) { m_Data = (char*)InDest; }
		void SetDataSize(uint32_t InSize) {  m_DataSize = InSize; }
		void SetType(const std::string& InType) { m_Type = InType; }

	protected:
		void* m_Data = nullptr;
		uint32_t m_DataSize = 0;
		std::string m_Type = "";
	};
}