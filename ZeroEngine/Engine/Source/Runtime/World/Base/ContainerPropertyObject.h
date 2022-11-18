#pragma once
#include "Core.h"
#include "PropertyObject.h"


namespace Zero
{
	class UContainerProperty : public UProperty
	{
	public:
		UContainerProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize);
	public:
		virtual UProperty* AddItem() = 0;
		virtual bool RemoveTailItem() { return false; }
		virtual bool RemoveAllItem() { return false; }

		virtual bool RemoveTailProperty();
		virtual void RemoveAllPreperties();
		
		virtual void UpdateProperty() = 0;

		virtual UProperty* AddProperty(void* Data, const std::string& Type, uint32_t ValueSize) = 0;
	protected:
		char* m_Data = nullptr;
		virtual void* AllocateData(uint32_t ValueSize, const std::string& ValueType);//Ìí¼ÓÊý¾Ý
		virtual void RemoveTailData(uint32_t ValueSize);
		virtual void RemoveAllData();


		uint32_t m_WholeDataSize = 0;
	};
}