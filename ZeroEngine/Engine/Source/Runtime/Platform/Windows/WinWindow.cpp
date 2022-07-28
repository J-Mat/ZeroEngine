#include "WinWindow.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/AppEvent.h"
#include "Utils.h"

namespace Zero
{
	#define TRANSFER_EVENT(FEvent, ...) if (WindowData.EventCallback == nullptr)\
								  	return 0; \
									auto Event = FEvent(__VA_ARGS__); \
								   	WindowData.EventCallback(Event);

	void FWinWindow::SetVSync(bool bEnabled)
	{
		
	}
	
	bool FWinWindow::IsVSync() const 
	{
		
	}
	
	void FWinWindow::Init(const FWindowsConfig& Config)
	{
		WindowData.Title = Config.Title;
		WindowData.Width = Config.Width;
		WindowData.Height = Config.Height;
		
		CORE_LOG_INFO("Creating windows {0} ({1}, {2})", Config.Title, Config.Width, Config.Height);

		#if defined(DEBUG) | defined(_DEBUG)
				_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		#endif
		
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hAppInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		if (!RegisterClass(&wc))
		{
			CORE_LOG_ERROR("Windows Window RegisterClass Failed.");
			return;
		}

		int Syswidth = GetSystemMetrics(SM_CXSCREEN);
		int Sysheight = GetSystemMetrics(SM_CYSCREEN);

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, (LONG)Config.Width, (LONG)Config.Height };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

		int Width = R.right - R.left;
		int Height = R.bottom - R.top;
		CORE_LOG_INFO("WinWindow Init finished.");
		
		
	
		
		
	}

	LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return FWinWindow::Main->MsgProc(hwnd, msg, wParam, lParam);
	}


	LRESULT CALLBACK FWinWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active. 
		case WM_ACTIVATE:
			return 0;
		case WM_SIZE:
		{
			WindowData.Width = LOWORD(lParam);
			WindowData.Height = HIWORD(lParam);
			TRANSFER_EVENT(FWindowResizeEvent, WindowData.Width, WindowData.Height);
			return 0;
		}
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			return 0;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			TRANSFER_EVENT(FWindowCloseEvent);
			return 0;
		}
		case WM_CHAR:
		{
			TRANSFER_EVENT(FKeyTypedEvent, (unsigned int)wParam);
		}
		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);
		case WM_GETMINMAXINFO:
			return 0;
		case WM_LBUTTONDOWN:
		{
			TRANSFER_EVENT(FMouseButtonPressedEvent, 0);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			TRANSFER_EVENT(FMouseButtonPressedEvent, 2);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			TRANSFER_EVENT(FMouseButtonPressedEvent, 1);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			TRANSFER_EVENT(FMouseButtonReleasedEvent, 0);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			TRANSFER_EVENT(FMouseButtonReleasedEvent, 2);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			TRANSFER_EVENT(FMouseButtonReleasedEvent, 1);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			TRANSFER_EVENT(FMouseMovedEvent, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			TRANSFER_EVENT(FMouseScrolledEvent, 0, (float)GET_WHEEL_DELTA_WPARAM(wParam) / 120);
			return 0;
		}
		case WM_KEYDOWN:
		{
			int PreviousDown = ((lParam >> 30) & 1) == 1;
			TRANSFER_EVENT(FKeyPressedEvent, (int)wParam, PreviousDown);
			return 0;
		}
		case WM_KEYUP:
		{
			TRANSFER_EVENT(FKeyReleasedEvent, (int)wParam);
			return 0;
		}
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}