#include "ClassInfoCollection.h"
#include "VariateProperty.h"
#include "ClassProperty.h"
#include "ArrayPropretyObject.h"
#include "MapPropretyObject.h"

namespace Zero
{
	UVariableProperty* FClassInfoCollection::AddVariableProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize)
    {
		UVariableProperty* Property = ConstructProperty<UVariableProperty>(ClassName, PropertyName, Data, PropertySize, PropertyType);
		Property->GetClassCollection().AddMeta("Category", ClassName);
		return Property;
    }

	UClassProperty* FClassInfoCollection::AddClassProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize)
	{
        auto* Property = ConstructProperty<UClassProperty>(
			ClassName,
			PropertyName,
			Data,
			PropertyType,
			PropertySize
			);
		Property->GetClassCollection().AddMeta("Category", ClassName);
		return Property;
	}

	UArrayProperty* FClassInfoCollection::AddArrayProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string ValueType, uint32_t InValueSize)
	{
        auto* Property = ConstructProperty<UArrayProperty>(
			ClassName, 
			PropertyName, 
			Data, 
			PropertyType, 
			PropertySize, 
			ValueType, 
			InValueSize
		);
		return Property;
	}

	UMapProperty* FClassInfoCollection::AddMapProperty(const std::string& ClassName, const std::string& PropertyName, void* Data, const std::string PropertyType, uint32_t PropertySize, std::string KeyType, uint32_t KeySize, std::string ValueType, uint32_t InValueSize)
	{
        auto* Property = ConstructProperty<UMapProperty>(
			ClassName, 
			PropertyName, 
			Data, 
			PropertyType, 
			PropertySize, 
			KeyType,
			KeySize,
			ValueType, 
			InValueSize
		);
		return Property;
	}

	UProperty* FClassInfoCollection::FindProperty(const std::string PropertyName)
	{
		UProperty* Property = HeadProperty;
		while (Property != nullptr)
		{
			if (Property->GetPropertyName() == PropertyName)
			{
				return Property;
			}
			
			Property = static_cast<UProperty*>(Property->Next);
		}
		return nullptr;
	}

	bool FClassInfoCollection::RemoveTailProperty()
	{
		UProperty* Property = HeadProperty;
		
		if (Property != nullptr)
		{
			if (Property == TailProprty)
			{
				delete Property;
				HeadProperty = TailProprty = nullptr;
				return true;
			}
			else
			{
				while (Property->Next != TailProprty)
				{
					Property = static_cast<UProperty*>(Property->Next);
				}
				delete TailProprty;
				TailProprty = Property;
				TailProprty->Next = nullptr;
				return true;
			}
		}
		return false;
	}

	uint32_t FClassInfoCollection::GetPropertyNum()
	{
		uint32_t Counter = 0;
		UProperty* Property = HeadProperty;
		while (Property != nullptr)
		{
			++Counter;
			Property = static_cast<UProperty*>(Property->Next);
		}
		return Counter;
	}

	void FClassInfoCollection::RemoveAllProperties()
	{
		UProperty* Property = HeadProperty;
		while (Property != nullptr)
		{
			UProperty* NextProperty = static_cast<UProperty*>(Property->Next);
			delete Property;
			Property = NextProperty;
		}
		HeadProperty = TailProprty = nullptr;
	}


	template<class T, typename ... Args>
	inline T* FClassInfoCollection::ConstructProperty(Args&& ... args)
	{
		T* Property = CreateObject<T>(m_Outer, std::forward<Args>(args)...);

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

	bool FClassInfoCollection::FindMetas(const std::string Key, std::string& Value)
	{
		auto Iter = m_Metas.find(Key);
		if (Iter != m_Metas.end())
		{
			Value = Iter->second;
			return true;
		}
		return false;
	}

	bool FClassInfoCollection::HasField(const std::string Field)
	{
		return m_Fields.contains(Field);
	}

	void FClassInfoCollection::AddMeta(const std::string& PropertyName, const std::string& Key, const std::string& Value)
	{
		if (UProperty* Property = FindProperty(PropertyName))
		{
			Property->GetClassCollection().AddMeta(Key, Value);
		}
		else 
		{
			AddMeta(Key, Value);
		}
	}
	void FClassInfoCollection::AddMeta(const std::string& Key, const std::string& Value)
	{
		if (Key.length() !=0 && Value.length() != 0)
		{
			m_Metas.insert({Key, Value});
		}
	}


	void FClassInfoCollection::AddField(const std::string& PropertyName, const std::string& Field)
	{
		if (UProperty* Property = FindProperty(PropertyName))
		{
			Property->GetClassCollection().AddField(Field);
		}
		else 
		{
			AddField(Field);
		}
	}

	void FClassInfoCollection::RemoveField(const std::string& PropertyName, const std::string& Field)
	{
		if (UProperty* Property = FindProperty(PropertyName))
		{
			Property->GetClassCollection().RemoveField(Field);
		}
		else 
		{
			RemoveField(Field);
		}
	}

	void FClassInfoCollection::AddField(const std::string& Field)
	{
		if (Field.length() != 0)
		{
			m_Fields.insert(Field);
		}
	}

	void FClassInfoCollection::RemoveField(const std::string& Field)
	{
		if (Field.length() != 0)
		{
			m_Fields.erase(Field);
		}
	}
}
