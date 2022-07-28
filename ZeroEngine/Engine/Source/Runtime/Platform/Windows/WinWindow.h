#pragma once
#include "Core.h"
#include "Core/Base/Windows.h"

namespace Zero
{
	class FWinWindow : public FWindowsBase
	{
	public:
		static FWinWindow* Main;
		FWinWindow(const FWindowsConfig& Config);
		virtual ~FWinWindow();

		void OnUpdate() override;
		
		inline unsigned GetWidth() const override {return WindowData.Width;}
		inline unsigned GetHeight() const override {return WindowData.Height;}
		

			// Window attributes
		inline void SetEventCallback(const EventCallbackFn& InCallback) override
		{
			WindowData.EventCallback = InCallback;
		}
		
		void SetVSync(bool bEnabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override {return (void*)hMainWnd;};
	private:
		virtual void Init(const FWindowsConfig& Config);
		virtual void Shutdown();

	private:
		struct FWindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallBackFn EventCallback;
		};
		FWindowData WindowData;
	protected:
		HINSTANCE hAppInst = nullptr; // application instance handle
		HWND      hMainWnd = nullptr; // main window handle
	public:
		LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		HWND* GetHWND() { return &hMainWnd; }
	};
}