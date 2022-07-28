#pragma once

#include "Core.h"

namespace Zero
{
	class FInput
	{
	public:
		static void Init();
		static void Destroy();

		inline static bool IsKeyPressed(int InKeycode) { return s_Instance->IsKeyPressedImpl(InKeycode); }
		inline static bool IsMouseButtonPressed(int InButton) { return s_Instance->IsMouseButtonPressedImpl(InButton); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int InKeycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int InButton) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static FInput* s_Instance;
	};
}