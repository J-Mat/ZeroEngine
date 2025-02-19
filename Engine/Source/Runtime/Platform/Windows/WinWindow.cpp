#include "WinWindow.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/ApplicationEvent.h"
#include "Utils.h"
#include "Platform/DX12/DX12RenderPipeline.h"
#include "Platform/DX12/MemoryManage/Resource/FrameResource.h"
#include "Render/RendererAPI.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace Zero
{
	#define TRANSFER_EVENT(FEvent, ...) if (m_WindowData.EventCallback == nullptr)\
								  	return 0; \
									auto Event = FEvent(__VA_ARGS__); \
								   	m_WindowData.EventCallback(Event);

	
	FWinWindow* FWinWindow::Main = nullptr;
	FWinWindow::FWinWindow(const FWindowsConfig& Config)
	{
		if (Main == nullptr)
		{
			Main = this;
		}
		Init(Config);
	}

	void FWinWindow::OnUpdate()
	{
		MSG msg = { 0 };

		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		FDX12RenderPipeline::Get().DrawFrame();
	}

	void FWinWindow::SetVSync(bool bEnabled)
	{
		m_WindowData.bVSync = bEnabled;
	}
	
	bool FWinWindow::IsVSync() const 
	{
		return m_WindowData.bVSync;
	}
	
	LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return FWinWindow::Main->MsgProc(hwnd, msg, wParam, lParam);
	}
	
	void FWinWindow::Init(const FWindowsConfig& Config)
	{
		m_WindowData.Title = Config.Title;
		m_WindowData.Width = Config.Width;
		m_WindowData.Height = Config.Height;
		m_WindowData.hAppInst = Config.hAppInst;
		m_WindowData.bScreeeMaxSize = Config.bScreenMaxSize;
		
		CORE_LOG_INFO("Creating windows {0} ({1}, {2})", Config.Title, Config.Width, Config.Height);

		#if defined(DEBUG) | defined(_DEBUG)
				_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		#endif
		
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_WindowData.hAppInst;
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

		std::wstring MainWndCaption = Utils::String2WString(Config.Title);
		if (m_WindowData.bScreeeMaxSize)
		{
			m_WindowData.hMainWnd = CreateWindow(L"MainWnd", MainWndCaption.c_str(),
				WS_MAXIMIZE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, m_WindowData.hAppInst, 0);
		}
		else
		{
			int Syswidth = GetSystemMetrics(SM_CXSCREEN);
			int Sysheight = GetSystemMetrics(SM_CYSCREEN);

			// Compute window rectangle dimensions based on requested client area dimensions.
			RECT R = { 0, 0, (LONG)Config.Width, (LONG)Config.Height };
			AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

			int Width = R.right - R.left;
			int Height = R.bottom - R.top;
			CORE_LOG_INFO("WinWindow Init finished.");

			m_WindowData.hMainWnd = CreateWindow(L"MainWnd", MainWndCaption.c_str(),
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, 0, 0, m_WindowData.hAppInst, 0);
		}
		
		if (!m_WindowData.hMainWnd)
		{
			CORE_LOG_ERROR("reateWindow Failed.");
			return;
		}
		
		m_Device = static_cast<FDX12Device*>(FGraphic::GraphicFactroy->CreateDevice().get())->AsShared();
		FDX12Device::Get()->CreateSwapChain(m_WindowData.hMainWnd);
		FDX12Device::Get()->FlushInitCommandList();
		FFrameResourcesManager::Get().Init();
		
		ShowWindow(m_WindowData.hMainWnd , SW_SHOW);
		
		UpdateWindow(m_WindowData.hMainWnd);
		
		SetVSync(false);
	}

	void FWinWindow::Shutdown()
	{
		DestroyWindow(m_WindowData.hMainWnd);
	}

	LRESULT CALLBACK FWinWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		{
			return true;
		}

		switch (msg)
		{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active. 
		case WM_ACTIVATE:
			return 0;
		case WM_SIZE:
		{
			m_WindowData.Width = LOWORD(lParam);
			m_WindowData.Height = HIWORD(lParam);
			TRANSFER_EVENT(FWindowResizeEvent, m_WindowData.Width, m_WindowData.Height);
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
			TRANSFER_EVENT(FMouseButtonDownEvent, 0);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			TRANSFER_EVENT(FMouseButtonDownEvent, 2);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			TRANSFER_EVENT(FMouseButtonDownEvent, 1);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			int X = GET_X_LPARAM(lParam);
			int Y = GET_Y_LPARAM(lParam);
			TRANSFER_EVENT(FMouseButtonReleasedEvent, 0, X, Y);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			int X = GET_X_LPARAM(lParam);
			int Y = GET_Y_LPARAM(lParam);
			TRANSFER_EVENT(FMouseButtonReleasedEvent, 2, X, Y);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			int X = GET_X_LPARAM(lParam);
			int Y = GET_Y_LPARAM(lParam);
			TRANSFER_EVENT(FMouseButtonReleasedEvent, 1, X, Y);
			return 0;
		}
		case WM_LBUTTONDBLCLK:
		{
			int X = GET_X_LPARAM(lParam);
			int Y = GET_Y_LPARAM(lParam);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			TRANSFER_EVENT(FMouseMovedEvent, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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