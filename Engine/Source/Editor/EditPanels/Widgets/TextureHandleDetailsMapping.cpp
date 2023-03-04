#include "TextureHandleDetailsMapping.h"
#include "Editor.h"
#include "Render/Moudule/Texture/TextureManager.h"

namespace Zero
{
	void FTextureHandleDetailsMapping::UpdateDetailsWidgetImpl(UProperty* Property, const char* ProperyTag)
	{
		static ImVec2 ButtonSize(64, 64);
		ImGuiStyle& Style = ImGui::GetStyle();
		FTextureHandle* TextureHandlePtr = Property->GetData<FTextureHandle>();
		*TextureHandlePtr = FTextureManager::Get().LoadTexture(TextureHandlePtr->TextureName);
		Ref<FTexture2D> Texture = FTextureManager::Get().GetTextureByHandle(*TextureHandlePtr);
		if (Texture == nullptr)
		{
			*TextureHandlePtr = FTextureManager::Get().GetDefaultTextureHandle();
			Texture = FTextureManager::Get().GetTextureByHandle(*TextureHandlePtr);
		}		
		ImTextureID TextureID = (ImTextureID)Texture->GetGuiShaderReseource();
		ImGui::Image(TextureID, ButtonSize, { 0,0 }, { 1,1 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(ASSET_PANEL_IMAGE))
			{
				std::string Path = (const char*)Payload->Data;
				auto Handle = FTextureManager::Get().LoadTexture(Path);
				*TextureHandlePtr = Handle;
				m_bEdited = true;
			}
			ImGui::EndDragDropTarget();
		}
	}
}
