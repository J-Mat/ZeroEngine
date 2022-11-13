#include "VariableDetailsMapping.h"

namespace Zero
{
    bool FVariableDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
    {
        m_bEdited = false;
        ImGui::Text(InProperty->GetEditorPropertyName());
        ImGui::SameLine();
        UpdateDetailsWidgetImpl(InProperty);
        return m_bEdited;
    }
}
