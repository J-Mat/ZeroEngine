#include "Color3DetailsMapping.h"

bool Zero::FColor3DetailsMapping::UpdateDetailsWidget(UProperty* Property)
{
    return ImGui::ColorEdit3(Property->GetEditorPropertyName(), (float*)Property->GetData<ZMath::FColor3>());
}
