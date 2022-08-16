#include "Application.h"
#include "Core/Base/Input.h"


namespace Zero
{
	FApplication* FApplication::s_Instance = nullptr;

	FApplication::FApplication(HINSTANCE hInst, const std::string& m_Name)
	{
		CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		// Init: Window
		Window = FWindows::Create(FWindowsConfig(hInst, m_Name));
		
		// Init: Input System
		FInput::Init();
		
		FrameTimer.reset(FFrameTimer::Create());
		FrameTimer->Reset();
	}

	void FApplication::Run()
	{
		while (bRunning)
		{
			FrameTimer->Tick();

			for (FLayer* Layer : LayerStack)
			{
				Layer->OnUpdate();
			}
			Window->OnUpdate();
		}
	}
	
	void FApplication::OnEvent(FEvent &Event)
	{
		FEventDispatcher Dispatcher(Event);
		Dispatcher.Dispatch<FWindowCloseEvent>(BIND_EVENT_FN(FApplication::OnWindowClosed));
		Dispatcher.Dispatch<FWindowResizeEvent>(BIND_EVENT_FN(FApplication::OnWindowResized));

		for (auto Iter = LayerStack.end(); Iter != LayerStack.begin();)
		{
			(*--Iter)->OnEvent(Event);
			if (Event.bHandled)
			{
				break;
			}
		}
	}
	
	void FApplication::OnDraw()
	{
		for (auto Layer : LayerStack)
		{
			Layer->OnDraw();
		}
	}
	
	void FApplication::PushLayer(FLayer* Layer)
	{
		LayerStack.PushLayer(Layer);
		Layer->OnAttach();
	}
	
	void FApplication::PushOverlay(FLayer* Overlay)
	{
		LayerStack.PushOverlay(Overlay);
		Overlay->OnAttach();
	}
	
	
	bool FApplication::OnWindowClosed(FWindowCloseEvent& Event)
	{
		bRunning = false;
		return true;
	}

	bool FApplication::OnWindowResized(FWindowResizeEvent& Event)
	{
		if (Event.GetWidth() == 0 || Event.GetHeight() == 0)
		{
			bMinimized = true;
			return false;
		}
		
		bMinimized = false;
		//m_Window->GetGraphicContext()->OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}