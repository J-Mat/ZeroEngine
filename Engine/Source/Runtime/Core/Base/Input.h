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
		inline static std::pair<int32_t, int32_t> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		inline static int32_t GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static int32_t GetMouseY() { return s_Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int InKeycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int InButton) = 0;
		virtual std::pair<int32_t, int32_t> GetMousePositionImpl() = 0;
		virtual int32_t GetMouseXImpl() = 0;
		virtual int32_t GetMouseYImpl() = 0;

	private:
		static FInput* s_Instance;
	};
}