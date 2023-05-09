#pragma once

#include "LightComponent.h"
#include "../TransformComponent.h"
#include "Render/Moudule/SceneCapture.h"


namespace Zero
{
	class UDirectLightComponnet : public ULightComponent
	{
	public:
		UDirectLightComponnet();
		virtual void PostInit();
		
		ZMath::mat4 GetProjectView();
		ZMath::mat4 GetProject() { return m_SceneCapture2D.GetSceneView().Proj; }
		ZMath::mat4 GetView() { return m_SceneCapture2D.GetSceneView().View; }
	
		FSceneCapture2D& GetSceneView() { return m_SceneCapture2D; }

		Ref<IShaderConstantsBuffer> GetCamera() { return m_SceneCapture2D.GetCamera();}
		
	private:
		void OnTransfromChanged(UTransformComponent* TransformComponent);
	private: 
		float m_Width = 80.0f;
		FSceneCapture2D m_SceneCapture2D;
	};
}