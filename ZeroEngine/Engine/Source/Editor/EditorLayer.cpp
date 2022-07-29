#include "EditorLayer.h"

namespace Zero
{
	FEditorLayer::FEditorLayer() :
		FLayer("EditorLayer")
	{
		
	}
	
	void FEditorLayer::OnAttach()
	{
		CLIENT_LOG_INFO("FEditorLayer::OnAttach");
	}
	
	void FEditorLayer::OnDetach()
	{
		
	}
	
	void FEditorLayer::OnUpdate()
	{
		
	}
	
	void FEditorLayer::OnEvent(FEvent& e)
	{
		
	}
}