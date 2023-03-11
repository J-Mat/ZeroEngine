#pragma once

#include "Core.h"
#include "RenderStage.h"
#include "Render/RenderConfig.h"


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
		void RawRenderLayer(ERenderLayer RenderLayerType);
		void ForwarLitRender();
	private:
		FCommandListHandle m_CommandListHandle;
		Ref<FRenderTarget2D> m_RenderTarget;
		Ref<FImageBasedLighting> m_IBLMoudule;
		bool m_bGenerateIrradianceMap = false;
	};
}