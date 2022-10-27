#include "DetailsMappingManager.h"
#include "ZeroEngine.h"

namespace Zero
{
    extern std::map<std::string, FClassID> g_ClassInfo;
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
            std::string CurrentClassName = Property->GetBelongClassName();
            
            if (CurrentClassName != Category)
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

        std::list<std::string> InheritLink;
        std::string ClassName = CoreObject->GetObjectName();
        std::string DerivedClassName;
        for (auto Iter = GetClassInfoMap().find(ClassName); 
            Iter != GetClassInfoMap().end();
            Iter = Iter = GetClassInfoMap().find(ClassName)
            )
        {
            InheritLink.push_front(ClassName);
            ClassName = Iter->second.DerivedClassName;
        }
       
        for (auto& ClassName : InheritLink)
        {
            if (m_CurrentProperty == nullptr)
            {
                break;
            }
            const std::string& CurrentClassName = m_CurrentProperty->GetBelongClassName();
            if (CurrentClassName == ClassName)
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
