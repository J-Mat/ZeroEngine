#include "EnumDetailsMapping.h"

namespace Zero
{
	int FEnumDetailsMapping::GetEnumIndex(const FEnumElement& EnumElement, int EnumValue)
	{
		for (int i = 0; i < EnumElement.EnumItemList.size(); ++i)
		{
			if (EnumElement.EnumItemList[i].second.Value == EnumValue)
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

		const char* ComboPreviewValue = EnumElement.EnumItemList[SelectIndex].first.c_str();
		if (EnumElement.EnumItemList[SelectIndex].second.Fields.contains("Invisible"))
		{ 
			ImGui::Text(ComboPreviewValue);
		}
		else if (ImGui::BeginCombo(ProperyTag, ComboPreviewValue, flags))
		{
			for (int i = 0; i < EnumElement.EnumItemList.size(); i++)
			{
				if (EnumElement.EnumItemList[i].second.Fields.contains("Invisible"))
				{ 
					continue;
				}
				const bool bSelected = (SelectIndex == i);
				if (ImGui::Selectable(EnumElement.EnumItemList[i].first.c_str(), bSelected))
				{
					SelectIndex = i;
					*EnumValue = EnumElement.EnumItemList[i].second.Value;
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
