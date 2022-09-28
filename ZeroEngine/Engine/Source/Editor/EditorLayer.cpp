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
		FRenderer::GetDevice()->PreInitWorld();
		InitEditPanel();
		BuildWorld();
	}

	void FEditorLayer::BuildWorld()
	{
		m_World = UWorld::CreateWorld();
		m_World->SetDevice(FRenderer::GetDevice());
		UWorld::SetCurrentWorld(m_World);
		
		//UCustomMeshActor* MeshActor = UActor::Create<UCustomMeshActor>(m_World, "cat", "backpack.obj");
		UCustomMeshActor* MeshActor = UActor::Create<UCustomMeshActor>(m_World, "cat", "sphere.fbx");
		//UCubeMeshActor* MeshActor = UActor::Create<UCubeMeshActor>(m_World);

		m_CameraController = CreateRef<FEditorCameraController>(m_World->GetCameraActor());

		Ref<FRenderStage> ForwardRendering = FForwardStage::Create();
		m_ScriptablePipeline->PushLayer(ForwardRendering);

		FShaderRegister::GetInstance().RegisterDefaultShader();
	}

	void FEditorLayer::InitEditPanel()
	{
		m_ContentBrowserPanel.Init();
	}
	
	void FEditorLayer::OnAttach()
	{
		CLIENT_LOG_INFO("FEditorLayer::OnAttach");
		FRenderer::GetDevice()->FlushInitCommandList();
		
		auto RenderTarget = TLibrary<FRenderTarget>::Fetch(FORWARD_STAGE);
		m_ViewportPanel.SetRenderTarget(RenderTarget);
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


	void FEditorLayer::OnGuiRender()
	{
		static bool bDockspaceOpen = true;
		static bool bFullscreen = true;
		static bool bPadding = false;
		static ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_None;
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (bFullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			DockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}
		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
			WindowFlags |= ImGuiWindowFlags_NoBackground;
		}
		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!bPadding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &bDockspaceOpen, WindowFlags);
		if (!bPadding)
			ImGui::PopStyleVar();
		if (bFullscreen)
			ImGui::PopStyleVar(2);
		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		//style.WindowMinSize.x = 350.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), DockspaceFlags);
		}
		//////////////////////////////////////////////
		// MenuBar
		//////////////////////////////////////////////
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+Shift+S"))
				{
				}

				if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
				

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		bool showdemo = true;
		ImGui::ShowDemoWindow(&showdemo);
		
		m_ViewportPanel.OnGuiRender();
		m_ContentBrowserPanel.OnGuiRender();

		ImGui::End();
	}
	
	void FEditorLayer::OnEvent(FEvent& e)
	{
		
	}
}