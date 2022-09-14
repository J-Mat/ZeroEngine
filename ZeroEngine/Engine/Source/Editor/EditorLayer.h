#pragma once

#include <ZeroEngine.h>


namespace Zero
{
	class FEditorCameraController;
	class FEditorLayer : public FLayer
	{
	public:
		FEditorLayer();
		virtual ~FEditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;
		virtual void OnDraw() override;
		void OnEvent(FEvent& e) override;
		
		void BuildWorld();
		
	private: 
		Ref<FRenderPipeline> m_ScriptablePipeline;
		UWorld* m_World = nullptr;
		Ref<FEditorCameraController> m_CameraController;
	};
}