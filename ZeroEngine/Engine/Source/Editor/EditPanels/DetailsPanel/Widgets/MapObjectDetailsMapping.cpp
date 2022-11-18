#include "MapObjectDetailsMapping.h"
#include "World/Base/MapPropretyObject.h"
#include "DetailsMappingManager.h"

namespace Zero
{
	void FMapObjectDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		UMapProperty* MapProperty = static_cast<UMapProperty*>(Property);
		if (ImGui::TreeNode(MapProperty->GetEditorPropertyName(), "%s", MapProperty->GetEditorPropertyName()))
		{
			if (ImGui::Button("+"))
			{
				MapProperty->AddItem();
				m_bEdited = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				MapProperty->RemoveTailItem();
				m_bEdited = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("-all"))
			{
				MapProperty->RemoveAllItem();
				m_bEdited = true;
			}

			UpdateWidget(Property->GetClassCollection().HeadProperty);

			ImGui::TreePop();
		}
	}

	void FMapObjectDetailsMapping::UpdateWidget(UProperty* KeyProperty)
	{
		while (KeyProperty != nullptr)
		{
			UProperty* ValueProperty = dynamic_cast<UProperty*>(KeyProperty->Next);
			Ref<FVariableDetailsMapping> VariableDetailsMapping = FDetailMappingManager::GetInstance().FindPropertyMapping(KeyProperty->GetPropertyType());
			if (VariableDetailsMapping != nullptr)
			{
				if (VariableDetailsMapping->UpdateDetailsWidget(KeyProperty))
				{
					m_bEdited = true;
				}
			}
			ImGui::Text("---");
			ImGui::SameLine();
			VariableDetailsMapping = FDetailMappingManager::GetInstance().FindPropertyMapping(ValueProperty->GetPropertyType());
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
