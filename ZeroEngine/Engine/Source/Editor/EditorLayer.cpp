#include "EditorLayer.h"
#include "Render/Pipeline/ForwardStage.h"
#include "Platform/Windows/WinWindow.h"
#include "Render/RendererAPI.h"
#include "EditorCameraController.h"

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
		
		UMeshActor* MeshActor = UActor::Create<UMeshActor>(m_World);
		FRenderer::GetDevice()->FlushInitCommandList();

		m_CameraController = CreateRef<FEditorCameraController>(m_World->GetCameraActor());

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
	
	void FEditorLayer::OnEvent(FEvent& e)
	{
		
	}
}