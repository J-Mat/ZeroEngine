#include "DetailsMappingManager.h"



namespace Zero
{

    void FDetailMappingManager::RegisterVariableMapping(EPropertyType PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping)
    {
        m_ProppertyMapping.insert({ PropertyType, PropertyDetailsMapping });
    }


    Ref<FVariableDetailsMapping> FDetailMappingManager::FindPropertyMapping(EPropertyType PropertyType)
    {
        auto Iter = m_ProppertyMapping.find(PropertyType);
        if (Iter != m_ProppertyMapping.end())
        {
            return  Iter->second;
        }
        return nullptr;
    }

    bool FDetailMappingManager::UpdatePropertyWidgets(UVariableProperty* Property)
    {
        ImGui::Separator();
        while (Property != nullptr)
        {
            Ref<FVariableDetailsMapping> PropertyDetailsMapping = FindPropertyMapping(Property->GetType());
            PropertyDetailsMapping->UpdateDetailsWidget(Property);
            Property->GetOuter()->PostEdit(Property);

            //Property = dynamic_cast<UProperty*>(Property->Next);
        }
        ImGui::Separator();
        return false;
    }
    bool FDetailMappingManager::UpdateClassWidgets(UCoreObject* CoreObject)
    {
        for (auto& ClassName : CoreObject->GetClassCollection().GetInheritLink())
        {
            
        }
        return false;
    }
}
