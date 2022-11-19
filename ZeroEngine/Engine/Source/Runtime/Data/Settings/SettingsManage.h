#pragma once
#include "Core/Base/PublicSingleton.h"
#include "World/Base/ObjectGenerator.h"
#include "Setting.h"

namespace Zero
{
	class FSettingManager : public IPublicSingleton<FSettingManager>
	{
	public:
		void RegisterSetting();
	private:
		std::map <std::string, USetting*> m_AllSettings;
	}
}