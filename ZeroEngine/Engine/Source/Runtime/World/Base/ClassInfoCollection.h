#pragma once
#include "CoreObject.h"
#include "PropertyObject.h"
#include "ObjectGenerator.h"


namespace Zero
{
	class UCoreObject;
	class FClassInfoCollection
	{
	public:
		FClassInfoCollection() = default;
		
		UCoreObject* AddProperty(const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize);

		template<class T>
		inline T* ConstructProperty(
			const std::string& PropertyName, void* Data, uint32_t PropertySize, const std::string&  PropertyType)
		{
			UProperty* Property = CreateObject<UProperty>(m_Outer, Data, PropertySize, PropertyType);
			Property->SetName(PropertyName);
		
			if (HeadProperty == nullptr)
			{
				HeadProperty = Property;
				TailProprty = HeadProperty;
			}
			else
			{
				TailProprty->Next = Property;
				TailProprty = Property;
			}
			
			return Property;
		}
		UCoreObject* m_Outer;
		UProperty* HeadProperty = nullptr;
		UProperty* TailProprty = nullptr;;
	};
}