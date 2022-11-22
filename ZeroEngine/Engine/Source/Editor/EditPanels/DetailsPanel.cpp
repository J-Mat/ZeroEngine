#include "DetailsPanel.h"
#include "Widgets/DetailsMappingManager.h"
#include "Editor.h"

namespace Zero
{
	FDetailPanel::FDetailPanel()
	{
	

	}
	void FDetailPanel::OnGuiRender()
	{
		static Ref<FClassDetailsMapping> Mapping = FDetailMappingManager::GetInstance().FindClassMapping("UActor");
		ImGui::Begin("DetailPanel");
		if (FEditor::SelectedActor != nullptr)
		{
			Mapping->UpdateDetailsWidget(FEditor::SelectedActor);
		}
		
		ImGui::End();
	}
}
