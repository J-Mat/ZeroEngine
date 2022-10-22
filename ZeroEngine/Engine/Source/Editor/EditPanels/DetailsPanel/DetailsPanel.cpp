#include "DetailsPanel.h"
#include "Widgets/Mappings/DetailsMappingManager.h"
#include "Widgets/Mappings/BoolDetailsMapping.h"
#include "Widgets/Mappings/Vector3DetailsMapping.h"
#include "Widgets/Mappings/Vector4DetailsMapping.h"
#include "Widgets/Mappings/StringDetailsMapping.h"
#include "Editor.h"

namespace Zero
{
	FDetailPanel::FDetailPanel()
	{
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::vec3", FVector3DDetailsMapping::MakeDetailMapping());
	}
	void FDetailPanel::OnGuiRender()
	{
		ImGui::Begin("DetailPanel");
		
		if (FEditor::SelectedActor != nullptr)
		{
			FDetailMappingManager::GetInstance().UpdateClassWidgets(FEditor::SelectedActor);
		}
		
		ImGui::End();
	}
}
