#pragma once
#include "Core/Base/PublicSingleton.h"
#include "World/Object/ObjectGenerator.h"
#include "Settings.h"

namespace Zero
{
	class FSettingManager : public IPublicSingleton<FSettingManager>
	{
	public:
		void RegisterSetting(const char* SettingsName);

		template<class T> 
		T* FecthSettings(const std::string& SettingsName)
		{
			auto Iter = m_AllSettings.find(SettingsName);
			if (Iter != m_AllSettings.end())
			{
				return dynamic_cast<T*>(Iter->second);
			}
			return nullptr;
		}

		std::map<std::string, USettings*>& GetAllSettings() { return m_AllSettings; };
	private:
		std::map<std::string, USettings*> m_AllSettings;
	};
}