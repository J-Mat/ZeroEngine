#include "ClassInfoCollection.h"

namespace Zero
{
    UCoreObject* FClassInfoCollection::AddProperty(const std::string& PropertyName, void* Data, const std::string& PropertyType, uint32_t PropertySize)
    {
        return ConstructProperty<UProperty>(PropertyName, Data, PropertySize, PropertyType);
    }
    {
        return nullptr;
    }
}
