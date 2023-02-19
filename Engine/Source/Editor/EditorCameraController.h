#pragma once

#include <ZeroEngine.h>
#include "Core/Base/KeyCodes.h"


namespace Zero
{
	class FEditorCameraController
	{
	public:
		FEditorCameraController() = default;
		FEditorCameraController(UCameraActor* Camera);
		void Tick();
	private:
		UCameraActor* m_CameraActor = nullptr;
		void OnEvent(FEvent& e);
		float m_Speed = 10.0f;
		float m_MouseSensitivity = 0.1f;
		ZMath::FEulerAngle  m_CameraEulerAngle;
	};
}