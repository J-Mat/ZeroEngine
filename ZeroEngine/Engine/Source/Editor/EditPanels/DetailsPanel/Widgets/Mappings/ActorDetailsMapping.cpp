#include "ActorDetailsMapping.h"
#include "DetailsMappingManager.h"

namespace Zero
{
    void FActorDetailsMapping::UpdateDetailsWidget(UCoreObject* CoreObject)
    {
        UActor* Actor = static_cast<UActor*>(CoreObject);
        UComponent* RootComponpent = Actor->GetRootComponent();
        if (RootComponpent != nullptr)
        {
            ShowComponentObject(RootComponpent, 0);
        }
    }

    void FActorDetailsMapping::ShowComponentObject(UComponent* Component, int ID)
    {
        ImGui::PushID(ID);
        
        //FDetailMappingManager::GetInstance().UpdatePropertyWidgets(Component->GetClassCollection().HeadProperty);

        if (ImGui::TreeNode(Component->GetGuidString().c_str(), "%s", Component->GetName().c_str()))
        {
            for (auto* Child : Component->GetChidren())
            {
                ShowComponentObject(Child, ++ID);
            }
        }

        ImGui::PopID();
    }
}
