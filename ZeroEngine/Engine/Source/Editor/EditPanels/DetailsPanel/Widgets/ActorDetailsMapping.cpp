#include "ActorDetailsMapping.h"
#include "DetailsMappingManager.h"

namespace Zero
{
    void FActorDetailsMapping::UpdateDetailsWidget(UCoreObject* CoreObject)
    {
        UProperty* Property = CoreObject->GetClassCollection().HeadProperty;
        if (ImGui::TreeNodeEx("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TreePop();
        }
        UComponent* RootComponpent = static_cast<UActor*>(CoreObject)->GetRootComponent();
        if (RootComponpent != nullptr)
        {
            ShowComponentObject(RootComponpent);
        }
    }

    void FActorDetailsMapping::ShowComponentObject(UComponent* Component)
    {
        UProperty* Property = Component->GetClassCollection().HeadProperty;
        if (ImGui::TreeNodeEx(Component->GetObjectName(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            while (Property != nullptr)
            {
                Ref<FVariableDetailsMapping> VariableDetailsMapping =  FDetailMappingManager::GetInstance().FindPropertyMapping(Property->GetPropertyType());
                if (VariableDetailsMapping != nullptr)
                {
                    VariableDetailsMapping->UpdateDetailsWidget(Property);
                    Property->GetOuter()->PostEdit(Property);
                }
                Property = dynamic_cast<UProperty*>(Property->Next);
            }
            ImGui::TreePop();
        }
    }
}
