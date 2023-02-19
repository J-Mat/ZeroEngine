#pragma once

#include "Core.h"
#include "Core/Base/Input.h"

namespace Zero
{
	class FWinInput : public FInput
	{
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual int32_t GetMouseXImpl() override;
		virtual int32_t GetMouseYImpl() override;
		virtual std::pair<int32_t, int32_t> GetMousePositionImpl() override;
	};
}