#include "DetailsMappingManager.h"
#include "ZeroEngine.h"

namespace Zero
{
    extern std::map<std::string, FClassID> g_ClassInfo;
    void FDetailMappingManager::RegisterVariableMapping(std::string PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping)
    {
        m_VariableMapping.insert({ PropertyType, PropertyDetailsMapping });
    }

    void FDetailMappingManager::RegisterClassMapping(Ref<FClassDetailsMapping> ActorDetailsMapping)
    {
        m_ActorDetailsMapping = ActorDetailsMapping;
    }

    Ref<FVariableDetailsMapping> FDetailMappingManager::FindPropertyMapping(std::string PropertyType)
    {
        auto Iter = m_VariableMapping.find(PropertyType);
        if (Iter != m_VariableMapping.end())
        {
            return  Iter->second;
        }
        return nullptr;
    }


    bool FDetailMappingManager::UpdateClassWidgets(UCoreObject* CoreObject)
    {
        m_ActorDetailsMapping->UpdateDetailsWidget(CoreObject);
        return true;
    }
}
