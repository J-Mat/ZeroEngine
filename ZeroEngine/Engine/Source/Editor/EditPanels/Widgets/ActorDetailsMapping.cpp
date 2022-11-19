#include "ActorDetailsMapping.h"
#include "DetailsMappingManager.h"

namespace Zero
{
    void FActorDetailsMapping::UpdateDetailsWidgetImpl(UCoreObject* CoreObject)
    {
        UProperty* Property = CoreObject->GetClassCollection().HeadProperty;
        if (ImGui::TreeNodeEx("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            while (Property != nullptr)
            {
                Ref<FVariableDetailsMapping> VariableDetailsMapping = FDetailMappingManager::GetInstance().FindPropertyMapping(Property->GetPropertyType());
                if (VariableDetailsMapping != nullptr && !Property->GetClassCollection().HasField("Invisible"))
                {
                    if (VariableDetailsMapping->UpdateDetailsWidget(Property))
                    {
                        Property->GetOuter()->PostEdit(Property);
                    }
                }
                Property = dynamic_cast<UProperty*>(Property->Next);
            }
            ImGui::TreePop();
        }
        UActor* Actor = static_cast<UActor*>(CoreObject);
        std::vector<UComponent*>& Components = Actor->GetAllComponents();
        for (auto* Component : Components)
        {
            ShowComponentObject(Component);
        }
    }

    void FActorDetailsMapping::ShowComponentObject(UComponent* Component)
    {
        UProperty* Property = Component->GetClassCollection().HeadProperty;
        if (Property == nullptr)
        {
            return;
        }
        if (ImGui::TreeNodeEx(Component->GetObjectName(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            while (Property != nullptr)
            {
                Ref<FVariableDetailsMapping> VariableDetailsMapping =  FDetailMappingManager::GetInstance().FindPropertyMapping(Property->GetPropertyType());
                if (VariableDetailsMapping != nullptr && !Property->GetClassCollection().HasField("Invisible"))
                {
                    if (VariableDetailsMapping->UpdateDetailsWidget(Property))
                    {
                        Property->GetOuter()->PostEdit(Property);
                    }
                }
                Property = dynamic_cast<UProperty*>(Property->Next);
            }
            ImGui::TreePop();
        }
    }
}
