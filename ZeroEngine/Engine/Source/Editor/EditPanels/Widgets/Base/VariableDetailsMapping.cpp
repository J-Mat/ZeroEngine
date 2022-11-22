#include "VariableDetailsMapping.h"

namespace Zero
{
    bool FVariableDetailsMapping::UpdateDetailsWidget(UProperty* Property, bool bDisplayTag)
    {
        static std::string PropertyTag;
        PropertyTag = std::format("##{0}", Property->GetGuidString());
        m_bEdited = false;
        if (bDisplayTag)
        {
            ImGui::BeginTable("table_nested1", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable);
		    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100);
		    ImGui::TableSetupColumn("Property");
            ImGui::TableNextColumn();
            ImGui::Text(Property->GetEditorPropertyName());
            ImGui::TableNextColumn();
        }
        UpdateDetailsWidgetImpl(Property, PropertyTag.c_str());
        if (bDisplayTag)
        {
            ImGui::EndTable();
        }
        return m_bEdited;
    }
}
