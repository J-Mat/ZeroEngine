#include "LightManager.h"
#include "Render/Moudule/FrameConstantsManager.h"
#include "World/Component/LightComponent/DirectLightComponent.h"
#include "World/Component/TransformComponent.h"

namespace  Zero
{
	FLightManager::FLightManager()
	{
		m_DirectLightProperties = { "Color", "Intensity", "ProjView", "Direction"};
	}

	void FLightManager::AddDirectLight(ULightActor* Light)
	{
		m_DirectLights.push_back(Light);
	}

	void FLightManager::ClearLights()
	{
		m_DirectLights.clear();
	}

	void FLightManager::Tick()
	{
		auto& Buffer = FFrameConstantsManager::GetInstance().GetShaderConstantBuffer();
		for (size_t i = 0; i < m_DirectLights.size(); ++i)
		{
			auto* Light = m_DirectLights[i];
			UDirectLightComponnet*  DirectLightComponent =  Light->GetComponent<UDirectLightComponnet>();
			Buffer->SetInt("DirectLightNum", int(m_DirectLights.size()));
			UTransformComponent* TransformComponent = Light->GetComponent<UTransformComponent>();
			uint32_t PropertyIndex = 0;

			std::string PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetFloat3(PropertyStr, DirectLightComponent->GetLightColor());

			PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetFloat(PropertyStr, DirectLightComponent->GetLightIntensity());

			PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetMatrix4x4(PropertyStr, DirectLightComponent->GetViewProject());

			PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetFloat3(PropertyStr, TransformComponent->GetForwardVector());
		//std::cout << "dir: " << TransformComponent->GetForwardVector() <<
		}
	}
}
