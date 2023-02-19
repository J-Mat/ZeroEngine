#pragma once

#include "Core.h"
#include "Core/Framework/Layer.h"


namespace Zero
{
	class FGuiLayer : public FLayer
	{
	public:
		FGuiLayer();
		virtual ~FGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;
		virtual void OnDraw() override;
		void OnEvent(FEvent& e) override;
	protected:
		virtual void PlatformInit() = 0;
		virtual void NewFrameBegin() = 0;
		virtual void DrawCall() = 0 ;
		virtual void PlatformDestroy() = 0;
	};
}