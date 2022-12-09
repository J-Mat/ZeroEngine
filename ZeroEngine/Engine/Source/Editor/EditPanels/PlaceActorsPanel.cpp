#include "PlaceActorsPanel.h"
#include "Editor.h"

namespace Zero
{
    void FPlaceActorsPanel::Init()
    {
        m_ActroItems.push_back({ FEditor::CreateIcon("EditorRes/cube.png"), "UCubeMeshActor" });
        m_ActroItems.push_back({ FEditor::CreateIcon("EditorRes/sphere.png"), "USphereMeshActor" });
        m_ActroItems.push_back({ FEditor::CreateIcon("EditorRes/direct_light.png"), "UDirectLightActor" });
        m_ActroItems.push_back({ FEditor::CreateIcon("EditorRes/bulb.png"), "UPointLightActor" });
    }

	void FPlaceActorsPanel::OnGuiRender()
	{
		ImGui::Begin("PlaceActorPanel");
		ImGui::BeginChild("OutLine Panel", ImVec2(150, 0), true);
		ImGuiStyle& Style = ImGui::GetStyle();  

        int Selected = -1;
        // selectable list
        auto pos = ImGui::GetCursorPos();   

        static ImVec2 ButtonSize(16, 16);
        for (int i = 0; i < m_ActroItems.size(); ++i)
        {
            std::string ActorName = m_ActroItems[i].ActorName;
            ImGui::PushID(i);
            if (ImGui::Selectable("", i == Selected))
            {
                Selected = i;
            }
            ImVec4 Tint = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            ImTextureID TextureID = (ImTextureID)m_ActroItems[i].IconTexture->GetGuiShaderReseource();
			if (ImGui::BeginDragDropSource())
			{
				ImGui::Image(TextureID, ImVec2(64, 64), {0,0}, {1,1}, Style.Colors[ImGuiCol_WindowBg], Tint);
				const char* ItemPath = ActorName.c_str();
				ImGui::SetDragDropPayload(PLACEOBJ_PANEL, ActorName.c_str(), ActorName.length() + 1);
				ImGui::EndDragDropSource();
			}
            ImGui::SameLine(); 
            ImGui::ImageButton(TextureID, ButtonSize, { 0,0 }, { 1,1 }, 0, Style.Colors[ImGuiCol_WindowBg], Tint);
            ImGui::SameLine(); 
            ImGui::Text(m_ActroItems[i].ActorName.c_str());
            ImGui::PopID();
        }

		ImGui::EndChild();
		ImGui::End();
	}
}
