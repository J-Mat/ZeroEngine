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
			m_bEdited = ImGui::SliderFloat(
				ProperyTag,
				Property->GetData<float>(), 
				FloatSliderPtr->Min,
				FloatSliderPtr->Max,
				"%.3f",
				ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp
			);
		}
		else
		{
			m_bEdited = ImGui::SliderFloat(
				ProperyTag,
				Property->GetData<float>(),
				FloatSliderPtr->Max,
				FloatSliderPtr->Min,
				"%.3f",
				ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp
			);
		}
		
		std::string CheckboxTag = std::format("{0}checkbox", ProperyTag);
		ImGui::SameLine(); 
		ImGui::Text("Enable");
		ImGui::SameLine(); 
		m_bEdited |= ImGui::Checkbox(CheckboxTag.c_str(), &FloatSliderPtr->bEnableEdit);

		ImGuiInputTextFlags Flag = 0;
		if (FloatSliderPtr->bEnableEdit)
		{
			m_bEdited |= ImGui::InputFloat("##Min", &FloatSliderPtr->Min, 0.0f, 0.0f, "%.3f");
			ImGui::SameLine(); 
			m_bEdited |= ImGui::InputFloat("##Max", &FloatSliderPtr->Max, 0.0f, 0.0f, "%.3f");
		}
		else
		{
			ImGui::Text("%f --- %f", FloatSliderPtr->Min, FloatSliderPtr->Max);
		}
		ImGui::EndGroup();
	}
}
