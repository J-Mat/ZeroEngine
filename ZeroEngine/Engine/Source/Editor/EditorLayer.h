#pragma once

#include <ZeroEngine.h>

namespace Zero
{
	class FEditorLayer : public FLayer
	{
	public:
		FEditorLayer();
		virtual ~FEditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate() override;
		void OnEvent(FEvent& e) override;
	};
}