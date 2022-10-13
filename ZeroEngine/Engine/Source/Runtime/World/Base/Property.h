#pragma once
#include "Core.h"
#include "Field.h"

namespace Zero
{
	enum EPropertyType
	{
		PT_Unknown,
		PT_Int,
		PT_Float,
		PT_Double,
		PT_Vec3,
		PT_Vec4,
		PT_Color,
		PT_Vector,
		PT_Map,
	};

	class UProperty : public UField
	{
		using Supper = UField;
	public: 
		UProperty(void* Data, uint32_t DataSize, EPropertyType PropertyType);

		template<class T>
		T* GetData() const
		{
			return (T*)Data;
		}
		
		uint32_t GetDataSize() { return m_DataSize; }
		void* GetData() const { return m_Data; }
		const EPropertyType& GetType() const { return m_PropertyType; }

		void InitializeValue(void* InDest) { m_Data = (char*)InDest; }
		void SetDataSize(uint32_t InSize) {  m_DataSize = InSize; }
		void SetType(const EPropertyType& InType) { m_PropertyType = InType; }

	protected:
		void* m_Data = nullptr;
		uint32_t m_DataSize = 0;
		EPropertyType m_PropertyType = PT_Unknown;
	};
}