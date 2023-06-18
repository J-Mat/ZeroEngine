#pragma once

#include "Core.h"
#include "../Base/Windows.h"
#include "./Layer.h"
#include "./LayerStack.h"
#include "../Base/FrameTimer.h"
#include "../Events/MouseEvent.h"
#include "../Events/keyEvent.h"
#include "../Events/ApplicationEvent.h"

namespace Zero
{
	struct FApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int Index) const
		{
			CORE_ASSERT(Index < Count, "Index < Count");
			return Args[Index];
		}
	};

	class FWinWindow;
	class FApplication
	{
	public:
		FApplication(HINSTANCE hInst, const std::string& name = "Zero Enigne");
		virtual ~FApplication() = default;
		
		void Run();

		void OnEvent(FEvent &Event);
		void OnAwake();
		void OnDraw();
		void OnEndFrame();
		void PostDraw();
		void OnGuiRender();
		void OnResourceDestroy();
		
		void PushLayer(FLayer* Layer);
		void PushOverlay(FLayer* Overlay);
		FLayerStack& GetLayerStack() { return m_LayerStack; }

		inline FWindow& GetWindow() { return *m_Window.get(); }
		inline static FApplication& Get() { return *s_Instance; }
		inline FFrameTimer* GetFrameTimer() { return m_FrameTimer.get(); }
	
	private:
		bool OnWindowClosed(FWindowCloseEvent& Event);
		bool OnWindowResized(FWindowResizeEvent& Event);
	
	private:
		Ref<FWindow> m_Window;
		Scope<FFrameTimer> m_FrameTimer;
		
		bool bRunning = true;
		FLayerStack m_LayerStack;
		
		static FApplication* s_Instance;
		
		bool m_bMinimized = false;
	};

	// Defined in client
	FApplication* CreateApplication(HINSTANCE hInst);
}