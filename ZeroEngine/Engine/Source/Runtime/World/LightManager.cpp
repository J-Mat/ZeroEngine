#include "LightManager.h"
#include "Render/Moudule/FrameConstantsManager.h"
#include "World/Component/LightComponent/DirectLightComponent.h"
#include "World/Component/LightComponent/PointLightComponent.h"
#include "World/Component/TransformComponent.h"

namespace  Zero
{
	FLightManager::FLightManager()
	{
		m_DirectLightProperties = {"Color", "Intensity", "Direction", "ProjView" };
		m_PointLightProperties = {"Color", "Intensity"};
	}

	void FLightManager::AddDirectLight(UDirectLightActor* Light)
	{
		m_DirectLights.push_back(Light);
	}

	void FLightManager::AddPointLight(UPointLightActor* Light)
	{
		m_PointLights.push_back(Light);
	}

	void FLightManager::ClearLights()
	{
		m_DirectLights.clear();
		m_PointLights.clear();
	}

	void FLightManager::Tick()
	{
		auto& Buffer = FFrameConstantsManager::GetInstance().GetShaderConstantBuffer();
		for (int32_t i = 0; i < m_DirectLights.size(); ++i)
		{
			auto* LightActor = m_DirectLights[i];
			LightActor->SetParameter("DirectLightIndex", EShaderDataType::Int, &i);
			UDirectLightComponnet*  DirectLightComponent =  LightActor->GetComponent<UDirectLightComponnet>();
			Buffer->SetInt("DirectLightNum", int(m_DirectLights.size()));
			UTransformComponent* TransformComponent = LightActor->GetComponent<UTransformComponent>();
			uint32_t PropertyIndex = 0;

			std::string PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetFloat3(PropertyStr, DirectLightComponent->GetLightColor());

			PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetFloat(PropertyStr, DirectLightComponent->GetLightIntensity());

			PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetFloat3(PropertyStr, TransformComponent->GetForwardVector());

			PropertyStr = std::format("DirectLights[{0}].{1}", i, m_DirectLightProperties[PropertyIndex++]);
			Buffer->SetMatrix4x4(PropertyStr, DirectLightComponent->GetViewProject());
		}

		for (int32_t i = 0; i < m_PointLights.size(); ++i)
		{
			auto* LightActor = m_PointLights[i];
			LightActor->SetParameter("PointLightIndex", EShaderDataType::Int, &i);
			UPointLightComponnet*  DirectLightComponent =  LightActor->GetComponent<UPointLightComponnet>();
			Buffer->SetInt("PointLightNum", int(m_PointLights.size()));
			UTransformComponent* TransformComponent = LightActor->GetComponent<UTransformComponent>();
			uint32_t PropertyIndex = 0;

			std::string PropertyStr = std::format("PointLights[{0}].{1}", i, m_PointLightProperties[PropertyIndex++]);
			Buffer->SetFloat3(PropertyStr, DirectLightComponent->GetLightColor());

			PropertyStr = std::format("PointLights[{0}].{1}", i, m_PointLightProperties[PropertyIndex++]);
			Buffer->SetFloat(PropertyStr, DirectLightComponent->GetLightIntensity());
		}
	}
}
