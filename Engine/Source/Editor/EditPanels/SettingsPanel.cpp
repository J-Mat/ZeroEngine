#include "SettingsPanel.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"
#include "Widgets/DetailsMappingManager.h"


namespace Zero
{
	void FSettingsPanel::OnGuiRender()
	{
		static Ref<FClassDetailsMapping> Mapping = FDetailMappingManager::Get().FindClassMapping("USettings");
		static std::map<std::string, USettings*>& AllSettings = FSettingManager::Get().GetAllSettings();
		for (auto Iter : AllSettings)
		{
			if (ImGui::Begin(Iter.first.c_str()))
			{
				Mapping->UpdateDetailsWidget(Iter.second);
			}
			ImGui::End();
		}
	}
}
