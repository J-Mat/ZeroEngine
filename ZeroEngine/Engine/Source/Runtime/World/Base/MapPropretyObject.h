#pragma once
#include "ContainerPropertyObject.h"

namespace Zero
{
	class UMapProperty : public UContainerProperty
	{
	public:
		UMapProperty(
			const std::string& ClassName, 
			const std::string& PropertyName, 
			void* Data, 
			const std::string PropertyType,
			uint32_t PropertySize,
			std::string KeyType,
			uint32_t KeySize,
			std::string ValueType, 
			uint32_t InValueSize
		);
		
		virtual UProperty* AddProperty(void* Data, const std::string& Type, uint32_t ValueSize) override;
		virtual void UpdateProperty() override;
		virtual UProperty* AddItem() override;
		virtual bool RemoveTailItem() override;
		virtual bool RemoveAllItem() override;

		const std::string& GetKeyType() { return m_KeyType; }
		void SetKeyType(const std::string& Type) { m_KeyType = Type; }

		const std::string& GetValueType() { return m_ValueType; }
		void SetValueType(const std::string& Type) { m_ValueType = Type; }
		
		uint32_t GetKeySize() { return m_KeySize; }
		void SetKeySize(uint32_t Size) { m_KeySize = Size; }

		uint32_t GetValueSize() { return m_ValueSize; }
		void SetValueSize(uint32_t Size) { m_ValueSize = Size; }
	private:
		std::string m_ValueType;
		uint32_t m_ValueSize;

		std::string m_KeyType;
		uint32_t m_KeySize;
	};
}