#include "TextureDetailsMapping.h"
#include "Editor.h"

namespace Zero
{
	bool FTextureDetailsMapping::UpdateDetailsWidget(UProperty* InProperty)
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
			if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload(ASSEST_PANEL_FILE))
			{
				std::string Path = (const char*)Payload->Data;
				std::cout << Path << std::endl;
				//*TextureHandlePtr = Path;
			}
			ImGui::EndDragDropTarget();
		}
		return true;
	}
}
