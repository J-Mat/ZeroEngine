#include "EditorLayer.h"
#include "Render/Pipeline/ForwardStage.h"
#include "Platform/Windows/WinWindow.h"
#include "Render/Moudule/ShaderRegister.h"
#include "Render/RendererAPI.h"

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
		m_World = UWorld::CreateWorld();
		m_World->SetDevice(FRenderer::GetDevice());
		UWorld::SetCurrentWorld(m_World);

		
		//UMeshActor* MeshActor = UActor::CreateActor<UMeshActor>(m_World);

		//FShaderRegister::GetInstance().RegisterDefaultShader(m_World->GetDevice().get());
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
	}

	void FEditorLayer::OnDraw()
	{
		m_ScriptablePipeline->Run();
	}
	
	void FEditorLayer::OnEvent(FEvent& e)
	{
		
	}
}