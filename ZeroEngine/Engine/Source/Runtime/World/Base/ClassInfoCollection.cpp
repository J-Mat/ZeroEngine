#include "ClassInfoCollection.h"

namespace Zero
{
    UCoreObject* FClassInfoCollection::AddProperty(const std::string& PropertyName, void* Data, uint32_t PropertySize, const EPropertyType PropertyType)
    {
        return ConstructProperty<UProperty>(PropertyName, Data, PropertySize, PropertyType);
    }
}
