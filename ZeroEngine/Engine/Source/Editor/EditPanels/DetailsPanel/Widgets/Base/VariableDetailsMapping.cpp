#include "VariableDetailsMapping.h"

namespace Zero
{
    bool FVariableDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
    {
        ImGui::Text(InProperty->GetEditorPropertyName());
        ImGui::SameLine();
        return false;
    }
}
