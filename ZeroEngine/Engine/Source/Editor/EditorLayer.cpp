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
	
	void FEditorLayer::OnEvent(FEvent& e)
	{
		
	}
}