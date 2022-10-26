#include "DetailsPanel.h"
#include "Widgets/Mappings/DetailsMappingManager.h"
#include "Widgets/Mappings/BoolDetailsMapping.h"
#include "Widgets/Mappings/Vector3DetailsMapping.h"
#include "Widgets/Mappings/Vector4DetailsMapping.h"
#include "Widgets/Mappings/StringDetailsMapping.h"
#include "Widgets/Mappings/Rotation3DetailsMapping.h"
#include "Editor.h"

namespace Zero
{
	FDetailPanel::FDetailPanel()
	{
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::vec3", FVector3DDetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("std::string",FStringDetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::FRotation",FRotation3DetailsMapping::MakeDetailMapping());
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
