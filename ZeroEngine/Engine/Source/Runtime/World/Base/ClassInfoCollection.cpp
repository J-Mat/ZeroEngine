#include "ClassInfoCollection.h"
#include "VariateProperty.h"
#include "ClassProperty.h"

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
        auto* Property =  ConstructProperty<UClassProperty>(ClassName, Data, PropertySize, PropertyType);
		Property->GetClassCollection().AddMeta("Category", ClassName);
		return Property;
	}

	UProperty* FClassInfoCollection::FindProperty(const std::string PropertyName)
	{
		UProperty* Property = HeadProperty;
		while (Property != nullptr)
		{
			if (Property->GetObjectName() == PropertyName)
			{
				return Property;
			}
			
			Property = static_cast<UProperty*>(Property->Next);
		}
		return nullptr;
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

	bool FClassInfoCollection::ExitField(const std::string Field)
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

	void FClassInfoCollection::AddField(const std::string& Field)
	{
		if (Field.length() != 0)
		{
			m_Fields.insert(Field);
		}
	}
}
