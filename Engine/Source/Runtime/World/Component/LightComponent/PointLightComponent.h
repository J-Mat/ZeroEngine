#pragma once

#include "LightComponent.h"
#include "../TransformComponent.h"
#include "Render/Moudule/SceneCapture.h"

namespace Zero
{
	class UPointLightComponnet : public ULightComponent
	{
	public:
		UPointLightComponnet() = default;
		virtual void PostInit();
		virtual void Tick() override;

		UPROPERTY()
		float m_Far = 100.0f;	
			

		float GetRange() { return m_Far; }
	
		const FSceneCaptureCube& GetSceneCaptureCube() { return m_SceneCaptureCube; }
	private:
		void OnTransfromChanged(UTransformComponent* TransformComponent);
	private: 
		float m_Near = 0.01f;	
		float m_Width = 1024;
		FSceneCaptureCube m_SceneCaptureCube;
	};
}