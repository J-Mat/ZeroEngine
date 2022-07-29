#pragma once

#include "Core.h"
#include "WinInput.h"

namespace Zero
{
    bool FWinInput::IsKeyPressedImpl(int keycode)
    {
        return false;
    }
    bool FWinInput::IsMouseButtonPressedImpl(int button)
    {
        return false;
    }
    float FWinInput::GetMouseXImpl()
    {
        return 0.0f;
    }
    float FWinInput::GetMouseYImpl()
    {
        return 0.0f;
    }
    std::pair<float, float> FWinInput::GetMousePositionImpl()
    {
        return std::pair<float, float>();
    }
}