#pragma once

#include <ZeroEngine.h>
#include "EditPanels/ViewportPanel.h"
#include "EditPanels/ContentBrowserPanel.h"
#include "EditPanels/OutlinePanel.h"
#include "EditPanels/DetailsPanel/DetailsPanel.h"
#include "EditPanels/PlaceActorsPanel.h"

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
		void OnEvent(FEvent& Event) override;

		void SaveScene();
		void OpenScene();
		
		
		void BuildWorld() ;
		void InitEditPanel();

	private:
		bool MouseButtonPressed(FMouseButtonReleasedEvent& Event);
		
	private: 
		Ref<FRenderPipeline> m_ScriptablePipeline;
		UWorld* m_World = nullptr;
		Ref<FEditorCameraController> m_CameraController;
		FViewportPanel m_ViewportPanel;
		FContentBrowserPanel m_ContentBrowserPanel;
		FOutlinePanel m_OutlinePanel;
		FDetailPanel m_DetailPanel;
		FPlaceActorsPanel m_PlaceActorPanel;
	
	};
}