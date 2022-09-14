#include "EditorCameraController.h"

namespace Zero
{
	FEditorCameraController::FEditorCameraController(UCameraActor* Camera)
		: m_CameraActor(Camera)
	{
	}

	void FEditorCameraController::Tick()
	{
		if (m_CameraActor == nullptr)
		{
			return;
		}
		float DeltaTime = FApplication::Get().GetFrameTimer()->GetDeltaTime();
		if (FInput::IsKeyPressed(Zero_KEY_W))
		{
			m_CameraActor->MoveForward({0.0f, 0.0f, DeltaTime * 10});
		}
		else if (FInput::IsKeyPressed(Zero_KEY_S))
		{
			m_CameraActor->MoveForward({0.0f, 0.0f, -DeltaTime * 10});
		}

		if (FInput::IsKeyPressed(Zero_KEY_A))
		{
			m_CameraActor->MoveForward({ DeltaTime * 10, 0.0f, 0.0f});
		}
		else if (FInput::IsKeyPressed(Zero_KEY_D))
		{
			m_CameraActor->MoveForward({ -DeltaTime * 10, 0.0f, 0.0f});
		}
	}

	void FEditorCameraController::OnEvent(FEvent& e)
	{
	}
	
}
