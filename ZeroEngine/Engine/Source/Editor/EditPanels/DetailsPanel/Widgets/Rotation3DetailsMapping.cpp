#include "Rotation3DetailsMapping.h"

namespace Zero
{

	bool FRotation3DetailsMapping::UpdateDetailsWidget(UProperty* Property)
	{
		ZMath::FRotation* Value = Property->GetData<ZMath::FRotation>();
		
		ZMath::FRotation ValueDegree = ZMath::degrees(*Value);

		ImGuiIO& io = ImGui::GetIO();
		auto BoldFont = io.Fonts->Fonts[0];

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(Property->GetPropertyName());
		ImGui::NextColumn();


		ImGui::BeginTable("table_padding", 3, ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoPadInnerX);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(BoldFont);


		if (ImGui::Button("Pitch", buttonSize))
			ValueDegree.x = 0;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Pitch", &ValueDegree.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();

		ImGui::TableSetColumnIndex(1);


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Yaw", buttonSize))
			ValueDegree.y = 0.0f;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Yaw", &ValueDegree.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::SameLine();

		ImGui::TableSetColumnIndex(2);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Roll", buttonSize))
			ValueDegree.z = 0.0f;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Roll", &ValueDegree.z, 0.1f, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleVar();

		ImGui::EndTable();

		ImGui::EndColumns();

		ZMath::FRotation ValueRadian = ZMath::radians(ValueDegree);
		Value->x = ValueRadian.x;
		Value->y = ValueRadian.y;
		Value->z = ValueRadian.z;

		return true;
		//return NumberWidget::ConstructFloat3Widget(Property);
	}
}
