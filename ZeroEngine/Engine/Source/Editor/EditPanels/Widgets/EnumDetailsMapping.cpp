#include "EnumDetailsMapping.h"

namespace Zero
{
	int FEnumDetailsMapping::GetEnumIndex(const FEnumElement& EnumElement, int EnumValue)
	{
		for (int i = 0; i < EnumElement.EnumInfoList.size(); ++i)
		{
			if (EnumElement.EnumInfoList[i].second == EnumValue)
			{
				return i;
			}
		}
		return -1;
	}

	void FEnumDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		int* EnumValue = Property->GetData<int>();
		static ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;

		auto Iter = FObjectGlobal::GetRegiterEnumMap().find(Property->GetPropertyType());
		FEnumElement EnumElement = Iter->second;

		int SelectIndex = GetEnumIndex(EnumElement, *EnumValue);
		CORE_ASSERT(SelectIndex != -1, "Invalid Enum Index!");

		const char* ComboPreviewValue = EnumElement.EnumInfoList[SelectIndex].first.c_str();  
		if (ImGui::BeginCombo(ProperyTag, ComboPreviewValue, flags))
		{
			for (int i = 0; i < EnumElement.EnumInfoList.size(); i++)
			{
				const bool bSelected = (SelectIndex == i);
				if (ImGui::Selectable(EnumElement.EnumInfoList[i].first.c_str(), bSelected))
				{
					SelectIndex = i;
					*EnumValue = EnumElement.EnumInfoList[i].second;
					m_bEdited = true;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
}
