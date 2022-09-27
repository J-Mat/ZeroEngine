#pragma once

#include <ZeroEngine.h>
#include "EditPanels/ViewportPanel.h"
#include "EditPanels/ContentBrowserPanel.h"

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
		virtual void OnGuiRender() override;
		void OnEvent(FEvent& e) override;
		
		void BuildWorld();
		void InitEditPanel();
		
	private: 
		Ref<FRenderPipeline> m_ScriptablePipeline;
		UWorld* m_World = nullptr;
		Ref<FEditorCameraController> m_CameraController;
		FViewportPanel m_ViewportPanel;
		FContentBrowserPanel m_ContentBrowserPanel;
	};
}