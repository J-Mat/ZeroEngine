#pragma once
#include "ContainerPropertyObject.h"

namespace Zero
{
	class UArrayProperty : public UContainerProperty
	{
	public:
		UArrayProperty(
			const std::string& ClassName, 
			const std::string& PropertyName, 
			void* Data, 
			const std::string PropertyType,
			uint32_t PropertySize,
			std::string ValueType, 
			uint32_t InValueSize
		);
		
		virtual UProperty* AddProperty(void* Data, const std::string& Type, uint32_t ValueSize);
		virtual void UpdateProperty() override;
		virtual UProperty* AddItem() override;
		virtual bool RemoveTailItem() override;
		virtual bool RemoveAllItem() override;

		void SetValueType(const std::string& Type) { m_ValueType = Type; }
		void SetValueSize(uint32_t Size) { m_ValueSize = Size; }

		const std::string& GetValueType() const { return m_ValueType; }
		uint32_t GetValueSize() const { return m_ValueSize; }

	private:
		std::string m_ValueType;
		uint32_t m_ValueSize;
	};
}