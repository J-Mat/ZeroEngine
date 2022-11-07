#pragma once

#include <ZeroEngine.h>
#include "EditPanels/BasePanel.h"

#define ASSEST_PANEL_OBJ "Assests_Obj"
#define ASSEST_PANEL_IMAGE "Assests_Image"
#define ASSEST_PANEL_FILE "Assests_File"
#define PLACEOBJ_PANEL "PlaceActor"

namespace Zero
{
	class FBasePanel;
	class FEditor
	{
	public:
		static Ref<FTexture2D> CreateIcon(const std::string&& FileName)
		{
			auto Icon = FRenderer::GraphicFactroy->CreateTexture2D(FileName);
			return Icon;
		}
		static UActor* SelectedActor;
		static std::map<std::string, Ref<FBasePanel>> AllPanels;
		static Ref<FBasePanel> GetPanelByName(const std::string& PanelName);
		static void RegisterPanel(const std::string& PanelName, Ref<FBasePanel> Panel);
		static void DrawAllPanels();

	};
}