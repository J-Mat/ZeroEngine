#include "EditorLayer.h"
#include "Render/Pipeline/ForwardStage.h"
#include "Platform/Windows/WinWindow.h"

namespace Zero
{
	FEditorLayer::FEditorLayer() :
		FLayer("EditorLayer")
	{
		m_ScriptablePipeline = CreateRef<FRenderPipeline>();
		m_World = CreateObject<UWorld>(nullptr);
		UWorld::SetCurrentWorld(m_World);
		m_World->SetDevice(FApplication::Get().GetWindow().GetDevice());
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