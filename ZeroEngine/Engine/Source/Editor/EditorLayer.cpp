#include "EditorLayer.h"
#include "Render/Pipeline/ForwardStage.h"
#include "Platform/Windows/WinWindow.h"
#include "Render/RendererAPI.h"
#include "EditorCameraController.h"
#include "World/Actor/SphereMeshActor.h"
#include "World/Actor/CubeMeshActor.h"
#include "World/Actor/CustomMeshActor.h"

namespace Zero
{
	FEditorLayer::FEditorLayer() :
		FLayer("EditorLayer")
	{
		m_ScriptablePipeline = CreateRef<FRenderPipeline>();
		BuildWorld();
	}

	void FEditorLayer::BuildWorld()
	{
		FRenderer::GetDevice()->PreInitWorld();
		m_World = UWorld::CreateWorld();
		m_World->SetDevice(FRenderer::GetDevice());
		UWorld::SetCurrentWorld(m_World);
		
		FRenderer::GraphicFactroy->CreateTexture2D(FRenderer::GetDevice().get(), "container.jpg");
		
		UCustomMeshActor* MeshActor = UActor::Create<UCustomMeshActor>(m_World, "cat", "backpack.obj");
		//UCustomMeshActor* MeshActor = UActor::Create<UCustomMeshActor>(m_World, "cat", "sphere.fbx");
		//UCubeMeshActor* MeshActor = UActor::Create<UCubeMeshActor>(m_World);
		FRenderer::GetDevice()->FlushInitCommandList();

		m_CameraController = CreateRef<FEditorCameraController>(m_World->GetCameraActor());

		FShaderRegister::GetInstance().RegisterDefaultShader(FRenderer::GetDevice().get());

	}
	
	void FEditorLayer::OnAttach()
	{
		CLIENT_LOG_INFO("FEditorLayer::OnAttach");
		Ref<FRenderStage> ForwardRendering = FForwardStage::Create();
		m_ScriptablePipeline->PushLayer(ForwardRendering);
	}
	
	void FEditorLayer::OnDetach()
	{
	}
	
	void FEditorLayer::OnUpdate()
	{
		UWorld::GetCurrentWorld()->Tick();
		m_CameraController->Tick();
	}

	void FEditorLayer::OnDraw()
	{
		m_ScriptablePipeline->Run();
	}

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	void FEditorLayer::OnGuiRender()
	{
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
		
	}
	
	void FEditorLayer::OnEvent(FEvent& e)
	{
		
	}
}