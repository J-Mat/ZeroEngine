#pragma once
#include "Core.h"
#include "PropertyObject.h"


namespace Zero
{
	class UContainerProperty : public UProperty
	{
	public:
		UContainerProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string ValueType, uint32_t InValueSize);
	public:
		virtual void* AddItem() { return NULL; }
		virtual bool RemoveTailItem() { return false; }
		virtual bool RemoveAllItem() { return false; }

		virtual bool AddProterty(void* Data, const std::string& Type, uint32_t ValueSize);
		virtual bool RemoveTailProperty();
		virtual void RemoveAllPreperties();
		
		virtual void UpdateProperty();

		void SetValueType(const std::string& Type) { m_ValueType = Type; }
		void SetValueSize(uint32_t Size) { m_ValueSize = Size; }

		uint32_t GetValueCount() const { return m_ValueCount; }
		void SetValueCount(uint32_t ValueCount) { m_ValueCount = ValueCount; }

		const std::string& GetValueType() const { return m_ValueType; }
		uint32_t GetValueSize() const { return m_ValueSize; }

#ifdef EDITOR_MODE 
		virtual bool UpdateEditorPropertyDetails(UProperty* Property);
#endif

	protected:
		char* m_Data = nullptr;
		virtual void* AllocateData();//Ìí¼ÓÊý¾Ý
		virtual bool RemoveTopData();
		virtual bool RemoveAllData();

		std::string m_ValueType = "";
		uint32_t m_ValueSize = 0;

		uint32_t m_ValueCount = 0;
	};
}