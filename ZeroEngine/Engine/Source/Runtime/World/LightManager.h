#pragma once
#include "Core/Base/PublicSingleton.h"
#include "World/Actor/LightActor.h"

namespace Zero
{
	class FLightManager : public IPublicSingleton<FLightManager>
	{
	public:
		FLightManager();
		void AddDirectLight(ULightActor* Light);
		void ClearLights();
		void Tick();
	private:
		std::vector<ULightActor*> m_DirectLights;
		std::vector<std::string> m_DirectLightProperties;
	};

}