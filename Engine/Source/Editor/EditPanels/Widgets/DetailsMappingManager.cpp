#include "DetailsMappingManager.h"
#include "ZeroEngine.h"

namespace Zero
{
    extern std::map<std::string, FClassID> g_ClassInfo;
    void FDetailMappingManager::RegisterVariableMapping(const std::string& PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping)
    {
        m_VariableMapping.insert({ PropertyType, PropertyDetailsMapping });
    }

    void FDetailMappingManager::RegisterClassMapping(const std::string& ClassType, Ref<FClassDetailsMapping> ClassDetailsMapping)
    {
        m_ClassDetailsMapping.insert({ ClassType, ClassDetailsMapping });
    }


    Ref<FVariableDetailsMapping> FDetailMappingManager::FindPropertyMapping(const std::string& PropertyType)
    {
        auto Iter = m_VariableMapping.find(PropertyType);
        if (Iter != m_VariableMapping.end())
        {
            return  Iter->second;
        }
        return nullptr;
    }

    Ref<FClassDetailsMapping> FDetailMappingManager::FindClassMapping(const std::string& ClassType)
    {
        auto Iter = m_ClassDetailsMapping.find(ClassType);
        if (Iter != m_ClassDetailsMapping.end())
        {
            return  Iter->second;
        }
        return nullptr;
    }
}
