#include "OutlinePanel.h"
#include "Editor.h"

namespace Zero
{
	void FOutlinePanel::OnGuiRender()
	{
		static int SelectedIndex = -1;
		ImGui::Begin("OutlinePanel");
		ImGui::BeginChild("OutLine Panel", ImVec2(150, 200), true);
		UWorld* World = UWorld::GetCurrentWorld();
		std::vector<UActor*> Actors = World->GetActors();
		for (int i = 0; i < Actors.size(); ++i)
		{
			UActor* Actor = Actors[i];
			if (ImGui::Selectable(Actor->GetTagName().c_str(), FEditor::SelectedActor == Actor))
			{
				SelectedIndex = i;
				FEditor::SelectedActor = Actor;
			}
		}
		ImGui::EndChild();
		ImGui::End();
	}
}
