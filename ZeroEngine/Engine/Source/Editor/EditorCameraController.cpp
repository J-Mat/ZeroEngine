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
			m_CameraActor->MoveLocal({0.0f, 0.0f, DeltaTime * m_Speed});
		}
		if (FInput::IsKeyPressed(Zero_KEY_S))
		{
			m_CameraActor->MoveLocal({0.0f, 0.0f, -DeltaTime * m_Speed});
		}

		if (FInput::IsKeyPressed(Zero_KEY_A))
		{
			m_CameraActor->MoveLocal({ -DeltaTime * m_Speed, 0.0f, 0.0f});
		}
		if (FInput::IsKeyPressed(Zero_KEY_D))
		{
			m_CameraActor->MoveLocal({ DeltaTime * m_Speed, 0.0f, 0.0f});
		}

		if (FInput::IsKeyPressed(Zero_KEY_Q))
		{
			m_CameraActor->MoveLocal({0.0f, -DeltaTime * m_Speed, 0.0f});
		}
		if (FInput::IsKeyPressed(Zero_KEY_E))
		{
			m_CameraActor->MoveLocal({ 0.0f, DeltaTime * m_Speed, 0.0f});
		}
		
		static int32_t PreX = 0, PreY = 0;
		static bool bMouseMove;
		static ZMath::vec3 PreRotaiton;

		if (FInput::IsMouseButtonPressed(Zero_MOUSE_BUTTON_RIGHT))
		{
			if (!bMouseMove)
			{
				bMouseMove = true;
				auto& MousePos = FInput::GetMousePosition();
				PreX = MousePos.second;
				PreY = MousePos.first;
				
				PreRotaiton =  ZMath::DegreeVec(m_CameraActor->GetRotation());
			}
			else
			{
				auto [CurY, CurX] = FInput::GetMousePosition();
				int32_t DiffX = CurX - PreX;
				int32_t DiffY = CurY - PreY;
				m_CameraEulerAngle.Pitch = DiffX * m_MouseSensitivity;
				m_CameraEulerAngle.Yaw = DiffY * m_MouseSensitivity;
				
				ZMath::vec3 CurRotation(
					PreRotaiton.x - m_CameraEulerAngle.Pitch, 
					PreRotaiton.y - m_CameraEulerAngle.Yaw,
					0.0f);
				
				m_CameraActor->SetRotation(CurRotation);
			}
		}
		else
		{
			bMouseMove = false;
		}
	}

	void FEditorCameraController::OnEvent(FEvent& e)
	{
	}
	
}
