#include "GuiLayer.h"
#include "Core/Framework/Application.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_dx12.h"

namespace Zero
{
	FGuiLayer::FGuiLayer() 
		: FLayer("GUI")
	{
	}
	void FGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		
		PlatformInit();
	}

	void FGuiLayer::OnDetach()
	{
	}

	void FGuiLayer::OnUpdate()
	{
		ImGuiIO& IO = ImGui::GetIO();
		auto& App = FApplication::Get();
		IO.DisplaySize = ImVec2((float)App.GetWindow().GetWidth(), (float)App.GetWindow().GetHeight());
		
		IO.DeltaTime = App.GetFrameTimer()->GetDeltaTime();
		
		NewFrameBegin();
		ImGui::NewFrame();

		FApplication::Get().OnGuiRender();

		ImGui::Render();
	}

	void FGuiLayer::OnDraw()
	{
		ImGui::EndFrame();
		DrawCall();
	}

	void FGuiLayer::OnEvent(FEvent& e)
	{
	}
}
