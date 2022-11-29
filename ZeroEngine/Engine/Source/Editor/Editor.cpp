#pragma once

#include "Editor.h"
#include "EditPanels/Widgets/DetailsMappingManager.h"
#include "EditPanels/Widgets/BoolDetailsMapping.h"
#include "EditPanels/Widgets/Vector3DetailsMapping.h"
#include "EditPanels/Widgets/Vector4DetailsMapping.h"
#include "EditPanels/Widgets/StringDetailsMapping.h"
#include "EditPanels/Widgets/Rotation3DetailsMapping.h"
#include "EditPanels/Widgets/FloatDetailsMapping.h"
#include "EditPanels/Widgets/ActorDetailsMapping.h"
#include "EditPanels/Widgets/Color3DetailsMapping.h"
#include "EditPanels/Widgets/Color4DetailsMapping.h"
#include "EditPanels/Widgets/TextureHandleDetailsMapping.h"	
#include "EditPanels/Widgets/MaterialHandleDetailsMapping.h"
#include "EditPanels/Widgets/AssignedFileDetailsMapping.h"
#include "EditPanels/Widgets/FloatSliderDetailsMapping.h"
#include "EditPanels/Widgets/ArrayObjectDetailsMapping.h"
#include "EditPanels/Widgets/MapObjectDetailsMapping.h"
#include "EditPanels/Widgets/IntDetailsMapping.h"
#include "EditPanels/Widgets/CoreObjectDetailsMapping.h"
#include "EditPanels/Widgets/EnumDetailsMapping.h"


namespace Zero
{
	Ref<FTexture2D> FEditor::m_HasMaterialtexutre = nullptr;
	Ref<FTexture2D> FEditor::m_NullMaterialtexutre = nullptr;
	UActor* FEditor::SelectedActor = nullptr;
	std::map<std::string, Ref<FBasePanel>> FEditor::AllPanels;
	void FEditor::RegisterDataUIMapings()
	{
		auto& DetailMappingManager = FDetailMappingManager::GetInstance();
		DetailMappingManager.RegisterVariableMapping("ZMath::vec3", FVector3DDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("std::string", FStringDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("ZMath::FRotation", FRotation3DetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("bool", FBoolDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("float", FFloatDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("ZMath::FColor3", FColor3DetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("ZMath::FColor4", FColor4DetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("FTextureHandle", FTextureHandleDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("FMaterialHandle", FMaterialHandleDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("FAssignedFile", FAssignedFileDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("FFloatSlider", FFloatSliderDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("std::vector", FArrayObjectDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("std::map", FMapObjectDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("int", FIntDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterVariableMapping("int32_t", FIntDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterClassMapping("UActor", FActorDetailsMapping::MakeDetailMapping());
		DetailMappingManager.RegisterClassMapping("USettings", FCoreObjectDetailsMapping::MakeDetailMapping());
		
		const std::map<std::string, FEnumElement>& EnumMap =  FObjectGlobal::GetRegiterEnumMap();
		for (auto Iter : EnumMap)
		{
			DetailMappingManager.RegisterVariableMapping(Iter.first, FEnumDetailsMapping::MakeDetailMapping());
		}
	}
	Ref<FBasePanel> FEditor::GetPanelByName(const std::string& PanelName)
	{
		return AllPanels[PanelName];
	}
	void FEditor::RegisterPanel(const std::string& PanelName, Ref<FBasePanel> Panel)
	{
		Panel->Init();
		AllPanels.insert({ PanelName, Panel });
	}
	void FEditor::DrawAllPanels()
	{
		for (auto Panel : AllPanels)
		{
			Panel.second->OnGuiRender();
		}
	}
}