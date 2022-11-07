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
#include "Widgets/TextureDetailsMapping.h"	
#include "Editor.h"

namespace Zero
{
	FDetailPanel::FDetailPanel()
	{
		auto& DetailMappingManager = FDetailMappingManager::GetInstance();
		DetailMappingManager.RegisterVariableMapping("ZMath::vec3", FVector3DDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("std::string",FStringDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("ZMath::FRotation",FRotation3DetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("bool", FBoolDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("float",FFloatDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("ZMath::FColor3", FColor3DetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("ZMath::FColor4", FColor4DetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("FTextureHandle", FTextureDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterClassMapping(FActorDetailsMapping::MakeDetailMapping());
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
