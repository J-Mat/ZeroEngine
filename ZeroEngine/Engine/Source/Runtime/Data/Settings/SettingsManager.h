#pragma once
#include "Core/Base/PublicSingleton.h"
#include "World/Base/ObjectGenerator.h"
#include "Settings.h"

namespace Zero
{
	class FSettingManager : public IPublicSingleton<FSettingManager>
	{
	public:
		void RegisterSetting(const char* SettingsName);
		std::map<std::string, USettings*>& GetAllSettings() { return m_AllSettings; };
	private:
		std::map<std::string, USettings*> m_AllSettings;
	};
}