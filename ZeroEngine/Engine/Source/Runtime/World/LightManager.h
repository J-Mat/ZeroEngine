#pragma once
#include "Core/Base/PublicSingleton.h"
#include "World/Actor/DirectLightActor.h"
#include "World/Actor/PointLightActor.h"

namespace Zero
{
	class FLightManager : public IPublicSingleton<FLightManager>
	{
	public:
		FLightManager();
		void AddDirectLight(UDirectLightActor* Light);
		void AddPointLight(UPointLightActor* Light);
		void ClearLights();
		void Tick();
	private:
		std::vector<UDirectLightActor*> m_DirectLights;
		std::vector<UPointLightActor*> m_PointLights;
		std::vector<std::string> m_DirectLightProperties;
		std::vector<std::string> m_PointLightProperties;
	};

}