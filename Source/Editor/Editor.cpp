#pragma once

#include "Editor.h"


namespace Zero
{
	Ref<FTexture2D> FEditor::m_HasMaterialtexutre = nullptr;
	Ref<FTexture2D> FEditor::m_NullMaterialtexutre = nullptr;
	UActor* FEditor::SelectedActor = nullptr;
	std::map<std::string, Ref<FBasePanel>> FEditor::AllPanels;
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