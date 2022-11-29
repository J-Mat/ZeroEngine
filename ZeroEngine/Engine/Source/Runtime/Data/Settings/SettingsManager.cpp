#include "SettingsManager.h"
#include "World/Base/ObjectGlobal.h"

namespace Zero
{
	void FSettingManager::RegisterSetting(const char* SettingsName)
	{
		if (m_AllSettings.contains(SettingsName))
		{
			return;
		}
		auto Iter = FObjectGlobal::GetClassInfoMap().find(SettingsName);
		if (Iter != FObjectGlobal::GetClassInfoMap().end())
		{
			auto* Settings = static_cast<USettings*>(Iter->second.Class->CreateDefaultObject());
			m_AllSettings.insert({Settings->GetObjectName(), Settings});
		}
	}
}
