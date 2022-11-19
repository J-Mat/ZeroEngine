#pragma once

#include "LightComponent.h"
#include "../TransformComponent.h"

namespace Zero
{
	class UDirectLightComponnet : public ULightComponent
	{
	public:
		UDirectLightComponnet() = default;
		virtual void PostInit();
		
		ZMath::mat4 GetViewProject();
	
	private:
		void OnTransfromChanged(UTransformComponent* TransformComponent);
	private: 
		float m_Width = 1024;
		ZMath::mat4 m_Projection = ZMath::orthoLH_NO(m_Width, m_Width, -m_Width, m_Width, -m_Width, m_Width);
		ZMath::mat4 m_View;
		ZMath::mat4 m_ViewProject;
	};
}