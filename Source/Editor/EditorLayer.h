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
		void RegisterEditPanel();

	private:
		bool MouseButtonPressed(FMouseButtonReleasedEvent& Event);
		
	private: 
		Ref<FRenderPipeline> m_ScriptablePipeline;
		UWorld* m_World = nullptr;
		Ref<FEditorCameraController> m_CameraController;
		Ref<FViewportPanel> m_ViewportPanel;
		Ref<FContentBrowserPanel> m_ContentBrowserPanel;
		Ref<FOutlinePanel> m_OutlinePanel;
		Ref<FDetailPanel> m_DetailPanel;
		Ref<FPlaceActorsPanel> m_PlaceActorPanel;
	
	};
}