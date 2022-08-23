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
		void OnResourceDestroy();
		
		void PushLayer(FLayer* Layer);
		void PushOverlay(FLayer* Overlay);
		FLayerStack& GetLayerStack() { return LayerStack; }

		inline FWinWindow& GetWindow() { return *m_Window; }
		inline static FApplication& Get() { return *s_Instance; }
		inline FFrameTimer* GetFrameTimer() { return FrameTimer.get(); }
	
	private:
		bool OnWindowClosed(FWindowCloseEvent& Event);
		bool OnWindowResized(FWindowResizeEvent& Event);
	
	private:
		Ref<FWinWindow> m_Window;
		Scope<FFrameTimer> FrameTimer;
		
		bool bRunning = true;
		FLayerStack LayerStack;
		
		static FApplication* s_Instance;
		
		bool bMinimized = false;
	};

	// Defined in client
	FApplication* CreateApplication(HINSTANCE hInst);
}