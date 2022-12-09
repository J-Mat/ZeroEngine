#pragma once

#include "LightComponent.h"
#include "../TransformComponent.h"

namespace Zero
{
	class UPointLightComponnet : public ULightComponent
	{
	public:
		UPointLightComponnet() = default;
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