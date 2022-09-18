#include "Application.h"
#include "Core/Base/Input.h"
#include "Render/RendererAPI.h"


namespace Zero
{
	FApplication* FApplication::s_Instance = nullptr;

	FApplication::FApplication(HINSTANCE hInst, const std::string& m_Name)
	{
		CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		// Init: Window
		FRenderer::SetRHI(ERHI::DirectX12);
		m_Window = FRenderer::GraphicFactroy->CreatePlatformWindow(FWindowsConfig(hInst, m_Name));
		m_Window->SetEventCallback(BIND_EVENT_FN(FApplication::OnEvent));
		
		// Init: Input System
		FInput::Init();
		
		m_FrameTimer.reset(FFrameTimer::Create());
		m_FrameTimer->Reset();
	}

	void FApplication::Run()
	{
		while (bRunning)
		{
			m_FrameTimer->Tick();

			for (FLayer* Layer : m_LayerStack)
			{
				Layer->OnUpdate();
			}
			m_Window->OnUpdate();
		}
	}
	
	void FApplication::OnEvent(FEvent &Event)
	{
		FEventDispatcher Dispatcher(Event);
		Dispatcher.Dispatch<FWindowCloseEvent>(BIND_EVENT_FN(FApplication::OnWindowClosed));
		Dispatcher.Dispatch<FWindowResizeEvent>(BIND_EVENT_FN(FApplication::OnWindowResized));

		for (auto Iter = m_LayerStack.end(); Iter != m_LayerStack.begin();)
		{
			(*--Iter)->OnEvent(Event);
			if (Event.bHandled)
			{
				break;
			}
		}
	}

	void FApplication::OnAwake()
	{
	}
	
	void FApplication::OnDraw()
	{
		for (auto Layer : m_LayerStack)
		{
			Layer->OnDraw();
		}
	}

	void FApplication::OnResourceDestroy()
	{
	}
	
	void FApplication::PushLayer(FLayer* Layer)
	{
		m_LayerStack.PushLayer(Layer);
	}
	
	void FApplication::PushOverlay(FLayer* Overlay)
	{
		m_LayerStack.PushOverlay(Overlay);
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
			m_bMinimized = true;
			return false;
		}
		
		m_bMinimized = false;
		m_Window->GetDevice()->Resize(Event.GetWidth(), Event.GetHeight());
		return false;
	}
}