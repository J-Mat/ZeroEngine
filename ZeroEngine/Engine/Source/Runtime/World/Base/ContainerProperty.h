#pragma once
#include "Core.h"
#include "PropertyObject.h"


namespace Zero
{
	class UContainerProperty : public UProperty
	{
	public:
		UContainerProperty(std::string ClassName, std::string PropertyName, void* Data, uint32_t DataSize, const std::string PropertyType);
	public:
		virtual void* AddItem() { return NULL; }
		virtual bool RemoveTailItem() { return false; }
		virtual bool RemoveAllItem() { return false; }


		void SetValueType(const std::string& Type) { m_ValueType = Type; }
		void SetValueSize(uint32_t Size) { m_ValueSize = Size; }

		uint32_t GetValueCount() const { return m_ValueCount; }
		void SetValueCount(uint32_t ValueCount) { m_ValueCount = ValueCount; }

		const std::string& GetValueType() const { return m_ValueType; }
		uint32_t GetValueSize() const { return m_ValueSize; }
	protected:
		char* m_Data;
		virtual void* AllocateData();//��������

		std::string m_ValueType = "";
		uint32_t m_ValueSize = 0;

		uint32_t m_ValueCount = 0;
	};
}