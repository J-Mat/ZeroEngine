#include "ClassInfoCollection.h"
#include "PropertyObject.h"

namespace Zero
{
    UProperty* FClassInfoCollection::AddProperty(const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize)
    {
        return ConstructProperty<UProperty>(PropertyName, Data, PropertySize, PropertyType);
    }


	template<class T>
	inline T* FClassInfoCollection::ConstructProperty(const std::string& PropertyName, void* Data, uint32_t PropertySize, const std::string& PropertyType)
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
}
