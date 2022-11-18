#include "VariableDetailsMapping.h"

namespace Zero
{
    bool FVariableDetailsMapping::UpdateDetailsWidget(UProperty* Property, bool bDisplayTag)
    {
        m_bEdited = false;
        if (bDisplayTag)
        {
            ImGui::Text(Property->GetEditorPropertyName());
            ImGui::SameLine();
        }
        static std::string PropertyTag;
        PropertyTag = std::format("##{0}", Property->GetGuidString());
        UpdateDetailsWidgetImpl(Property, PropertyTag.c_str());
        return m_bEdited;
    }
}
