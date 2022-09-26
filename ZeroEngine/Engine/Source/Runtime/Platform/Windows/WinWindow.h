#pragma once
#include "Core.h"
#include "Core/Base/Windows.h"
#include "../DX12/DX12Device.h"

namespace Zero
{
	class FWinWindow : public FWindow
	{
	public:
		LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		HWND* GetHWND() { return &m_WindowData.hMainWnd; }

		static FWinWindow* Main;
		FWinWindow(const FWindowsConfig& Config);
		virtual ~FWinWindow() = default;

		void OnUpdate() override;
		
		inline unsigned GetWidth() const override {return m_WindowData.Width;}
		inline unsigned GetHeight() const override {return m_WindowData.Height;}
		

			// Window attributes
		inline void SetEventCallback(const EventCallBackFn& InCallback) override
		{
			m_WindowData.EventCallback = InCallback;
		}
		
		void SetVSync(bool bEnabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override {return (void*)m_WindowData.hMainWnd;};

	private:
		virtual void Init(const FWindowsConfig& Config);
		virtual void Shutdown();

		struct FWindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool bScreeeMaxSize = true;
			bool VSync;
			HINSTANCE hAppInst = nullptr; // application instance handle
			HWND      hMainWnd = nullptr; // main window handle
			bool bVSync = true; 

			EventCallBackFn EventCallback;
		};
		FWindowData m_WindowData;
		
	};
}