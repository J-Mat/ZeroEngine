#include "MapObjectDetailsMapping.h"
#include "World/Object/MapPropretyObject.h"
#include "DetailsMappingManager.h"

namespace Zero
{
	void FMapObjectDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		UMapProperty* MapProperty = static_cast<UMapProperty*>(Property);
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if (ImGui::TreeNode(MapProperty->GetEditorPropertyName(), "%s", MapProperty->GetEditorPropertyName()))
		{
			if (!MapProperty->GetClassCollection().HasField("KeyUnEditable") && ImGui::Button("+"))
			{
				MapProperty->AddItem();
				m_bEdited = true;
			}

			ImGui::SameLine();
			if (!MapProperty->GetClassCollection().HasField("KeyUnEditable") && ImGui::Button("-"))
			{
				MapProperty->RemoveTailItem();
				m_bEdited = true;
			}

			ImGui::SameLine();
			if (!MapProperty->GetClassCollection().HasField("KeyUnEditable") && ImGui::Button("-all"))
			{
				MapProperty->RemoveAllItem();
				m_bEdited = true;
			}

			UpdateWidget(MapProperty, Property->GetClassCollection().HeadProperty);

			ImGui::TreePop();
		}
	}

	void FMapObjectDetailsMapping::UpdateWidget(UMapProperty* MapProperty, UProperty* KeyProperty)
	{
		while (KeyProperty != nullptr)
		{
			if (MapProperty->GetClassCollection().HasField("KeyUnEditable"))
			{
				KeyProperty->GetClassCollection().AddField("UnEditabe");
			}
			UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProperty->Next);
			Ref<FVariableDetailsMapping> VariableDetailsMapping = FDetailMappingManager::Get().FindPropertyMapping(KeyProperty->GetPropertyType());
			if (VariableDetailsMapping != nullptr)
			{
				if (VariableDetailsMapping->UpdateDetailsWidget(KeyProperty))
				{
					m_bEdited = true;
				}
			}
			ImGui::Text("---");
			ImGui::SameLine();
			VariableDetailsMapping = FDetailMappingManager::Get().FindPropertyMapping(ValueProperty->GetPropertyType());
			if (VariableDetailsMapping != nullptr)
			{
				if (VariableDetailsMapping->UpdateDetailsWidget(ValueProperty, false))
				{
					m_bEdited = true;
				}
			}
			KeyProperty = dynamic_cast<UProperty*>(ValueProperty->Next);
		}
	}
}
