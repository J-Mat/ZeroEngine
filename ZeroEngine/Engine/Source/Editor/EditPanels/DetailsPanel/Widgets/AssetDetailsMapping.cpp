#include "AssetDetailsMapping.h"
#include "DetailsMappingManager.h"

namespace Zero
{
    void FAssetDetailsMapping::UpdateDetailsWidgetImpl(UCoreObject* CoreObject)
    {
        UProperty* Property = CoreObject->GetClassCollection().HeadProperty;
        if (ImGui::TreeNodeEx("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            while (Property != nullptr)
            {
                Ref<FVariableDetailsMapping> VariableDetailsMapping =  FDetailMappingManager::GetInstance().FindPropertyMapping(Property->GetPropertyType());
                if (VariableDetailsMapping != nullptr && !Property->GetClassCollection().HasField("Invisible"))
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
