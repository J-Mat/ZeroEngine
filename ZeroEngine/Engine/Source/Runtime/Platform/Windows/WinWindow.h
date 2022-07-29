#pragma once
#include "Core.h"
#include "Core/Base/Windows.h"

namespace Zero
{
	class FWinWindow : public FWindows
	{
	public:
		static FWinWindow* Main;
		FWinWindow(const FWindowsConfig& Config);
		virtual ~FWinWindow() = default;

		void OnUpdate() override;
		
		inline unsigned GetWidth() const override {return WindowData.Width;}
		inline unsigned GetHeight() const override {return WindowData.Height;}
		

			// Window attributes
		inline void SetEventCallback(const EventCallBackFn& InCallback) override
		{
			WindowData.EventCallback = InCallback;
		}
		
		void SetVSync(bool bEnabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override {return (void*)WindowData.hMainWnd;};
	private:
		virtual void Init(const FWindowsConfig& Config);
		virtual void Shutdown();

	private:
		struct FWindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			HINSTANCE hAppInst = nullptr; // application instance handle
			HWND      hMainWnd = nullptr; // main window handle

			EventCallBackFn EventCallback;
		};
		FWindowData WindowData;
	protected:
	public:
		LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		HWND* GetHWND() { return &WindowData.hMainWnd; }
	};
}