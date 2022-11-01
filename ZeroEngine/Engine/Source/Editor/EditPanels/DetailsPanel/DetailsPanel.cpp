#include "DetailsPanel.h"
#include "Widgets/DetailsMappingManager.h"
#include "Widgets/BoolDetailsMapping.h"
#include "Widgets/Vector3DetailsMapping.h"
#include "Widgets/Vector4DetailsMapping.h"
#include "Widgets/StringDetailsMapping.h"
#include "Widgets/Rotation3DetailsMapping.h"
#include "Widgets/FloatDetailsMapping.h"
#include "Widgets/ActorDetailsMapping.h"
#include "Widgets/Color3DetailsMapping.h"
#include "Widgets/Color4DetailsMapping.h"
#include "Editor.h"

namespace Zero
{
	FDetailPanel::FDetailPanel()
	{
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::vec3", FVector3DDetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("std::string",FStringDetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::FRotation",FRotation3DetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("bool", FBoolDetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("float",FFloatDetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::FColor3", FColor3DetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterVariableMapping("ZMath::FColor4", FColor4DetailsMapping::MakeDetailMapping());
		FDetailMappingManager::GetInstance().RegisterClassMapping(FActorDetailsMapping::MakeDetailMapping());
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
