#include "MaterialHandleDetailsMapping.h"
#include "DetailsMappingManager.h"
#include "Editor.h"

namespace Zero
{
	void FMaterialHandleDetailsMapping::CheckMaterialAsset(FMaterialHandle* MaterialHandle)
	{
		if (m_MaterialAsset == nullptr || m_MaterialAsset->GetAssetName() != std::string(*MaterialHandle))
		{
			if (*MaterialHandle != "")
			{
				m_MaterialAsset = FAssetManager::GetInstance().LoadAsset<UMaterialAsset>(*MaterialHandle);
			}
		}
	}

    void FMaterialHandleDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
    {
		static ImVec2 ButtonSize(64, 64);
		FMaterialHandle* MaterialHandle = Property->GetData<FMaterialHandle>();
		CheckMaterialAsset(MaterialHandle);
		ImTextureID TextureID = (ImTextureID)FEditor::GetMaterialSlotTexture(m_MaterialAsset != nullptr)->GetGuiShaderReseource();
		ImGui::Image(TextureID, ButtonSize, { 0,0 }, { 1,1 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(ASSET_PANEL_MATERIAL))
			{
				std::string Path = (const char*)Payload->Data;
				*MaterialHandle = Path;
				CheckMaterialAsset(MaterialHandle);
				m_bEdited = true;
			}
			ImGui::EndDragDropTarget();
		}
		
		if (*MaterialHandle == "\0")
		{
			return;
		}
		
		UProperty* VariableProperty = m_MaterialAsset->GetClassCollection().HeadProperty;
        if (ImGui::TreeNodeEx("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            while (VariableProperty != nullptr)
            {
                Ref<FVariableDetailsMapping> VariableDetailsMapping = FDetailMappingManager::GetInstance().FindPropertyMapping(VariableProperty->GetPropertyType());
                if (VariableDetailsMapping != nullptr && !VariableProperty->GetClassCollection().HasField("Invisible"))
                {
					if (VariableDetailsMapping->UpdateDetailsWidget(VariableProperty))
					{
						VariableProperty->GetOuter()->PostEdit(VariableProperty);
						m_bEdited = true;
					}
                }
				VariableProperty = dynamic_cast<UProperty*>(VariableProperty->Next);
            }
            ImGui::TreePop();
        }
		if (ImGui::Button("Save Material"))
		{
			if (m_MaterialAsset)
			{
				FAssetManager::GetInstance().Serialize(m_MaterialAsset);
			}
		}
    }
}
