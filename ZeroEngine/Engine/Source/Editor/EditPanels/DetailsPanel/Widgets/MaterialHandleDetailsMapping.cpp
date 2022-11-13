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

    void FMaterialHandleDetailsMapping::UpdateDetailsWidgetImpl(UProperty* InProperty)
    {
		static ImVec2 ButtonSize(64, 64);
		FMaterialHandle* MaterialHandle = InProperty->GetData<FMaterialHandle>();
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
		
		UProperty* Property = m_MaterialAsset->GetClassCollection().HeadProperty;
        if (ImGui::TreeNodeEx("Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            while (Property != nullptr)
            {
                Ref<FVariableDetailsMapping> VariableDetailsMapping = FDetailMappingManager::GetInstance().FindPropertyMapping(Property->GetPropertyType());
                if (VariableDetailsMapping != nullptr && !Property->GetClassCollection().HasField("Invisible"))
                {
					if (VariableDetailsMapping->UpdateDetailsWidget(Property))
					{
						Property->GetOuter()->PostEdit(Property);
						m_bEdited = true;
					}
                }
                Property = dynamic_cast<UProperty*>(Property->Next);
            }
            ImGui::TreePop();
        }
    }
}
