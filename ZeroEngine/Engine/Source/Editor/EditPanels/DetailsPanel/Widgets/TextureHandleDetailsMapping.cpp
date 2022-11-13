#include "TextureHandleDetailsMapping.h"
#include "Editor.h"

namespace Zero
{
	void FTextureHandleDetailsMapping::UpdateDetailsWidgetImpl(UProperty* InProperty)
	{
		static ImVec2 ButtonSize(64, 64);
		ImGuiStyle& Style = ImGui::GetStyle();
		FTextureHandle* TextureHandlePtr = InProperty->GetData<FTextureHandle>();
		if (*TextureHandlePtr == "")
		{
			*TextureHandlePtr = "default";
		}
		Ref<FTexture2D> Texture = TLibrary<FTexture2D>::Fetch(*TextureHandlePtr);
		if (Texture == nullptr)
		{
			Texture = FRenderer::GraphicFactroy->CreateTexture2D(*TextureHandlePtr);
		}		
		ImTextureID TextureID = (ImTextureID)Texture->GetGuiShaderReseource();
		ImGui::Image(TextureID, ButtonSize, { 0,0 }, { 1,1 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(ASSET_PANEL_IMAGE))
			{
				std::string Path = (const char*)Payload->Data;
				*TextureHandlePtr = Path;
				m_bEdited = true;
			}
			ImGui::EndDragDropTarget();
		}
	}
}
