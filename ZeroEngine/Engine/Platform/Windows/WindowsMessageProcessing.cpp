#include "WindowsMessageProcessing.h"

LRESULT CALLBACK EngineWindowsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM LParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, LParam);
}

