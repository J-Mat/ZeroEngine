#pragma once

#include "Core.h"
#include "WinInput.h"
#include "Core/Framework/Application.h"
#include "Core/Base/Windows.h"

namespace Zero
{
    bool FWinInput::IsKeyPressedImpl(int Keycode)
    {
        if (GetAsyncKeyState(Keycode) & 0x8000)
        {
            return true;
        }
        return false;
    }

    bool FWinInput::IsMouseButtonPressedImpl(int Button)
    {
        if (GetAsyncKeyState(Button) & 0x8000)
        {
            return true;
        }

        return false;
    }
    float FWinInput::GetMouseXImpl()
    {
        POINT pt;
        BOOL bReturn = GetCursorPos(&pt);
        if (bReturn != 0)
        {
            if (ScreenToClient((HWND)FApplication::Get().GetWindow().GetNativeWindow(), &pt))
            {
                return pt.x;
            }
        }

        return 0.0f;
    }
    float FWinInput::GetMouseYImpl()
    {
        POINT pt;
        BOOL bReturn = GetCursorPos(&pt);
        if (bReturn != 0)
        {
            if (ScreenToClient((HWND)FApplication::Get().GetWindow().GetNativeWindow(), &pt))
            {
                return pt.y;
            }
        }

        return 0.0f;
    }
    std::pair<float, float> FWinInput::GetMousePositionImpl()
    {
        POINT pt;
        BOOL bReturn = GetCursorPos(&pt);
        if (bReturn != 0)
        {
            if (ScreenToClient((HWND)FApplication::Get().GetWindow().GetNativeWindow(), &pt))
            {
                return { pt.x ,pt.y };
            }
        }
        return { 0.0f, 0.0f };
    }
}