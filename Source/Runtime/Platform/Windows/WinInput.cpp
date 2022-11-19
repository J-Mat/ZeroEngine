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

    int32_t FWinInput::GetMouseXImpl()
    {
        POINT Point;
        BOOL bReturn = GetCursorPos(&Point);
        if (bReturn != 0)
        {
            if (ScreenToClient((HWND)FApplication::Get().GetWindow().GetNativeWindow(), &Point))
            {
                return Point.x;
            }
        }

        return 0;
    }

    int32_t FWinInput::GetMouseYImpl()
    {
        POINT Point;
        BOOL bReturn = GetCursorPos(&Point);
        if (bReturn != 0)
        {
            if (ScreenToClient((HWND)FApplication::Get().GetWindow().GetNativeWindow(), &Point))
            {
                return Point.y;
            }
        }

        return 0;
    }

    std::pair<int32_t, int32_t> FWinInput::GetMousePositionImpl()
    {
        POINT Point;
        BOOL bReturn = GetCursorPos(&Point);
        if (bReturn != 0)
        {
            if (ScreenToClient((HWND)FApplication::Get().GetWindow().GetNativeWindow(), &Point))
            {
                return { Point.x ,Point.y };
            }
        }
        return { 0, 0 };
    }
}