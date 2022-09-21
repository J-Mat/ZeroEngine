#pragma once

#include "Core.h"
#include "Core/Framework/Layer.h"


namespace Zero
{
	class FGuiLayer : public FLayer
	{
	public:
		FEditorLayer();
		virtual ~FEditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;
		virtual void OnDraw() override;
		void OnEvent(FEvent& e) override;
	};
}