#include "EditorLayer.h"
#include "Render/Pipeline/ForwardStage.h"
#include "Render/Pipeline/ShadowStage.h"
#include "Platform/Windows/WinWindow.h"
#include "EditorCameraController.h"
#include "Dialog/DialogUtils.h"
#include "Data/WorldSerializer.h"
#include "Editor.h"
#include "Colors.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"

namespace Zero
{
	FEditorLayer::FEditorLayer() :
		FLayer("EditorLayer")
	{
		m_ScriptablePipeline = CreateRef<FRenderPipeline>();
		FRenderer::GetDevice()->PreInitWorld();
		RegisterSettings();
		RegisterEditPanel();
		BuildWorld();
	}

	void FEditorLayer::BuildWorld()
	{
		m_World = UWorld::CreateWorld();
		m_World->SetDevice(FRenderer::GetDevice());
		UWorld::SetCurrentWorld(m_World);

		m_CameraController = CreateRef<FEditorCameraController>(m_World->GetMainCamera());

		RegisterPso();
		RegisterRenderStage();

		{
			Ref<FImage> Image = CreateRef<FImage>(ZConfig::GetAssestsFullPath("Texture\\DefaultTexture.png").string());
			FRenderer::GraphicFactroy->CreateTexture2D(Image, "default");
		}

		{
			Ref<FImage> Image = CreateRef<FImage>(ZConfig::GetAssestsFullPath("Texture\\pbr\\IBL_BRDF_LUT.png").string());
			FRenderer::GraphicFactroy->CreateTexture2D(Image, IBL_BRDF_LUT);
		}
	}

	void FEditorLayer::RegisterPso()
	{
		FPSORegister::GetInstance().RegisterDefaultPSO();
		FPSORegister::GetInstance().RegisterSkyboxPSO();
		FPSORegister::GetInstance().RegisterIBLPSO();
		FPSORegister::GetInstance().RegisterShadowPSO();
		FPSORegister::GetInstance().RegisterTestPSO();
	}

	void FEditorLayer::RegisterRenderStage()
	{
		Ref<FRenderStage> ShadowStage = FShadowStage::Create();
		m_ScriptablePipeline->PushLayer(ShadowStage);

		Ref<FRenderStage> ForwardRendering = FForwardStage::Create();
		m_ScriptablePipeline->PushLayer(ForwardRendering);
	}

	void FEditorLayer::RegisterEditPanel()
	{
		FEditor::RegisterDataUIMapings();
		FEditor::RegisterPanel("Content", m_ContentBrowserPanel = CreateRef<FContentBrowserPanel>());
		FEditor::RegisterPanel("Outline", m_OutlinePanel = CreateRef<FOutlinePanel>());
		FEditor::RegisterPanel("PlaceActor", m_PlaceActorPanel = CreateRef<FPlaceActorsPanel>());
		FEditor::RegisterPanel("Viewport", m_ViewportPanel = CreateRef<FViewportPanel>());
		FEditor::RegisterPanel("ShadowMap",  m_DebugViewportPanel = CreateRef<FDebugViewportPanel>());
		FEditor::RegisterPanel("Detail",  m_DetailPanel = CreateRef<FDetailPanel>());
		FEditor::RegisterPanel("Settings",  CreateRef<FSettingsPanel>());
	}

	void FEditorLayer::RegisterSettings()
	{
		FSettingManager::GetInstance().RegisterSetting(USceneSettings::StaticGetObjectName());
	}

	
	void FEditorLayer::OnAttach()
	{
		CLIENT_LOG_INFO("FEditorLayer::OnAttach");
		FRenderer::GetDevice()->FlushInitCommandList();
		
		auto MainViewportRenderTarget = TLibrary<FRenderTarget2D>::Fetch(RENDER_STAGE_FORWARD);
		m_ViewportPanel->SetRenderTarget(MainViewportRenderTarget);

		auto ShadowMapRenderTarget = TLibrary<FRenderTarget2D>::Fetch(RENDER_STAGE_SHADOWMAP_DEBUG);
		m_DebugViewportPanel->SetRenderTarget(ShadowMapRenderTarget);
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
					std::cout << "New\n";
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+Shift+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Load Scene", "Ctrl+O"))
				{
					OpenScene();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		bool showdemo = true;
		ImGui::ShowDemoWindow();

		FEditor::DrawAllPanels();

		ImGui::End();
	}
	
	void FEditorLayer::OnEvent(FEvent& Event)
	{
		FEventDispatcher Dispatcher(Event);
		//Dispatcher.Dispatch<FMouseButtonReleasedEvent>(BIND_EVENT_FN(FEditorLayer::MouseButtonPressed));
	}

	void FEditorLayer::SaveScene()
	{
		std::string Filepath = FFileDialog::SaveFile("Zero Scene (*.scene)\0*.scene\0", "scene");
		if (!Filepath.empty())
		{
			FWorldSerializer Serialzier(UWorld::GetCurrentWorld());
			Serialzier.Serialize(Filepath);
		}
	}

	void FEditorLayer::OpenScene()
	{
		FEditor::Reset();
		UWorld::GetCurrentWorld()->ClearAllActors();
		std::string Filepath = FFileDialog::OpenFile("Zero Scene (*.scene)\0*.scene\0", "scene");
		FWorldSerializer Serialzier(UWorld::GetCurrentWorld());
		std::cout << Serialzier.Deserialize(Filepath) << std::endl;
	}
	
	bool FEditorLayer::MouseButtonPressed(FMouseButtonReleasedEvent& Event)
	{
		return true;
	}
	
}