#include "DetailsMappingManager.h"



namespace Zero
{

    void FDetailMappingManager::RegisterVariableMapping(std::string PropertyType, Ref<FVariableDetailsMapping> PropertyDetailsMapping)
    {
        m_VariableMapping.insert({ PropertyType, PropertyDetailsMapping });
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


    bool FDetailMappingManager::UpdatePropertyWidgets(const std::string& Category, UProperty* Property)
    {
        while (Property != nullptr)
        {
            std::string CurrentCategory;
            bool bFindMetas = Property->GetClassCollection().FindMetas("Category", CurrentCategory);
            CLIENT_ASSERT(bFindMetas,  "Property Must have Category!");
            
            if (CurrentCategory != Category)
            {
                break;
            }
               
            Ref<FVariableDetailsMapping> VariableDetailsMapping = FindPropertyMapping(Property->GetPropertyType());
            if (VariableDetailsMapping != nullptr)
            {
                VariableDetailsMapping->UpdateDetailsWidget(Property);
                Property->GetOuter()->PostEdit(Property);
            }
            else
            {
                UCoreObject* Object = Property->GetData<UCoreObject>();
                UpdateClassWidgets(Object);
            }
            m_CurrentProperty = Property = dynamic_cast<UProperty*>(Property->Next);
        }
        ImGui::Separator();
        return false;
    }

    bool FDetailMappingManager::UpdateClassWidgets(UCoreObject* CoreObject)
    {
        m_CurrentProperty = CoreObject->GetClassCollection().HeadProperty;
        const std::vector<std::string>& Link = CoreObject->GetClassCollection().GetInheritLink();
        for (auto& ClassName : CoreObject->GetClassCollection().GetInheritLink())
        {
            if (m_CurrentProperty == nullptr)
            {
                break;
            }
            std::string CurrentCategory;
            bool bFindMetas = m_CurrentProperty->GetClassCollection().FindMetas("Category", CurrentCategory);
            if (CurrentCategory == ClassName)
            {
                if (ImGui::TreeNodeEx(ClassName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    UpdatePropertyWidgets(ClassName, m_CurrentProperty);
                    ImGui::TreePop();
                }
            }
        }
        return true;
    }
}
