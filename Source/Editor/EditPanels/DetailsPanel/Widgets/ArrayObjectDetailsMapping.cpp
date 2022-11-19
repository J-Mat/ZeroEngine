#include "ArrayObjectDetailsMapping.h"
#include "World/Base/ArrayPropretyObject.h"
#include "DetailsMappingManager.h"

namespace Zero
{
	void FArrayObjectDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		UArrayProperty* ArrayProperty = static_cast<UArrayProperty*>(Property);
		if (ImGui::TreeNode(ArrayProperty->GetEditorPropertyName(), "%s", ArrayProperty->GetEditorPropertyName()))
		{
			if (ImGui::Button("+"))
			{
				ArrayProperty->AddItem();
				m_bEdited = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				ArrayProperty->RemoveTailItem();
				m_bEdited = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("-all"))
			{
				ArrayProperty->RemoveAllItem();
				m_bEdited = true;
			}

			UpdateWidget(Property->GetClassCollection().HeadProperty);

			ImGui::TreePop();
		}
	}

	void FArrayObjectDetailsMapping::UpdateWidget(UProperty* Property)
	{
		while (Property != nullptr)
		{
			Ref<FVariableDetailsMapping> VariableDetailsMapping = FDetailMappingManager::GetInstance().FindPropertyMapping(Property->GetPropertyType());
			if (VariableDetailsMapping != nullptr && !Property->GetClassCollection().HasField("Invisible"))
			{
				if (VariableDetailsMapping->UpdateDetailsWidget(Property))
				{
					m_bEdited = true;
				}
			}
			Property = dynamic_cast<UProperty*>(Property->Next);
		}
	}
}
