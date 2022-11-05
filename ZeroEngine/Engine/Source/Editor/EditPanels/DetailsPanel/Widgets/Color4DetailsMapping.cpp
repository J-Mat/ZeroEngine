#include "Color4DetailsMapping.h"

bool Zero::FColor4DetailsMapping::UpdateDetailsWidget(UProperty* Property)
{
    return ImGui::ColorEdit3(Property->GetEditorPropertyName(), (float*)Property->GetData<ZMath::FColor4>());
}
