#pragma once

#include "Core.h"
#include "RenderStage.h"


namespace Zero
{
	class FRenderTargetCube;
	class FRenderItem;
	class FImageBasedLighting;
	class FForwardStage : public FRenderStage
	{
	public:
		PipeStageCreate(FForwardStage)
		FForwardStage() = default;
		virtual ~FForwardStage() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnDraw() override;
	private:
		Ref<FRenderTarget2D> m_RenderTarget;
		Ref<FImageBasedLighting> m_IBLMoudule;
		bool m_bGenerateIrradianceMap = false;
	};
}