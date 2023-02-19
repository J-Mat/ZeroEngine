#pragma once

#include "LightComponent.h"
#include "../TransformComponent.h"
#include "Render/RenderConfig.h"

namespace Zero
{
	class UDirectLightComponnet : public ULightComponent
	{
	public:
		UDirectLightComponnet();
		virtual void PostInit();
		
		ZMath::mat4 GetProjectView();
		ZMath::mat4 GetProject() { return m_SceneView.Proj; }
		ZMath::mat4 GetView() { return m_SceneView.View; }
	
		FSceneView& GetSceneView() { return m_SceneView; }
	private:
		void OnTransfromChanged(UTransformComponent* TransformComponent);
	private: 
		float m_Width = 80.0f;
		FSceneView m_SceneView;
	};
}