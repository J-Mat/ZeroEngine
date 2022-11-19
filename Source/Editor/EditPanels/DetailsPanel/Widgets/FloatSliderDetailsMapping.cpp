#include "FloatSliderDetailsMapping.h"

namespace Zero
{
	void FFloatSliderDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		static float ColumnWidth = 100.0f;
		ImGui::BeginGroup();
		FFloatSlider* FloatSliderPtr = Property->GetData<FFloatSlider>();
		if (FloatSliderPtr->Min < FloatSliderPtr->Max)
		{
			m_bEdited = ImGui::DragFloat(
				ProperyTag,
				Property->GetData<float>(), 
				FloatSliderPtr->Step, 
				FloatSliderPtr->Min,
				FloatSliderPtr->Max
			);
		}
		else
		{
			m_bEdited = ImGui::DragFloat(ProperyTag, Property->GetData<float>());
		}
		
		std::string CheckboxTag = std::format("{0}checkbox", ProperyTag);
		ImGui::SameLine(); 
		ImGui::Text("Enable");
		ImGui::SameLine(); 
		m_bEdited |= ImGui::Checkbox(CheckboxTag.c_str(), &FloatSliderPtr->bEnableEdit);

		ImGuiInputTextFlags Flag = 0;
		if (!FloatSliderPtr->bEnableEdit)
		{
			Flag |= ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly;
		}
		
		ImGuiIO& io = ImGui::GetIO();
		auto BoldFont = io.Fonts->Fonts[0];

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnWidth(0, ColumnWidth);
		ImGui::Text("Range");
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


		if (ImGui::Button("Min", buttonSize) && FloatSliderPtr->bEnableEdit)
		{
			FloatSliderPtr->Min = 0;
			m_bEdited = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		m_bEdited |= ImGui::InputFloat("##Min", &FloatSliderPtr->Min, 0.0f, 0.0f, "%.3f", Flag);
		ImGui::SameLine();

		ImGui::TableSetColumnIndex(1);


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Max", buttonSize) && FloatSliderPtr->bEnableEdit)
		{
			FloatSliderPtr->Max = 0.0f;
			m_bEdited = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		m_bEdited |= ImGui::InputFloat("##Max", &FloatSliderPtr->Max, 0.0f, 0.0f, "%.3f", Flag);
		ImGui::SameLine();

		ImGui::TableSetColumnIndex(2);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Step", buttonSize) && FloatSliderPtr->bEnableEdit)
		{
			FloatSliderPtr->Step = 0.01f;
			m_bEdited = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		m_bEdited |= ImGui::InputFloat("##Step", &FloatSliderPtr->Step, 0.0f, 0.0f, "%.3f", Flag);

		ImGui::PopStyleVar();

		ImGui::EndTable();

		ImGui::EndColumns();
		
		FloatSliderPtr->Value = ZMath::clamp(FloatSliderPtr->Value, FloatSliderPtr->Min, FloatSliderPtr->Max);
		ImGui::EndGroup();
	}
}
