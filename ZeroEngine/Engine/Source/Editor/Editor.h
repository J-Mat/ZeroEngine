#pragma once

#include <ZeroEngine.h>
#include "EditPanels/BasePanel.h"

#define ASSET_PANEL_OBJ "Assests_Obj"
#define ASSET_PANEL_IMAGE "Assests_Image"
#define ASSET_PANEL_SHADER "Assests_Shader"
#define ASSET_PANEL_FILE "Assests_File"
#define ASSET_PANEL_SCENE "Assests_Scene"
#define ASSET_PANEL_MATERIAL "Assests_Material"
#define PLACEOBJ_PANEL "PlaceActor"

namespace Zero
{
	class FBasePanel;
	class FEditor
	{
	public:
		static void RegisterDataUIMapings();
		static Ref<FTexture2D> CreateIcon(const std::string&& FileName)
		{
			auto Icon = FRenderer::GraphicFactroy->GetOrCreateTexture2D(FileName);
			return Icon;
		}
		static void Reset();
		static UActor* SelectedActor;
		static std::map<std::string, Ref<FBasePanel>> AllPanels;
		static Ref<FBasePanel> GetPanelByName(const std::string& PanelName);
		static void RegisterPanel(const std::string& PanelName, Ref<FBasePanel> Panel);
		static void DrawAllPanels();
		static Ref<FTexture2D> GetMaterialSlotTexture(bool bHasMaterial) { return bHasMaterial ? m_HasMaterialtexutre : m_NullMaterialtexutre; }
		static Ref<FTexture2D> m_HasMaterialtexutre;
		static Ref<FTexture2D> m_NullMaterialtexutre;
	};
}