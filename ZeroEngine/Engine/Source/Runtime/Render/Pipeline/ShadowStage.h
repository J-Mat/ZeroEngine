#pragma once

#include "Core.h"
#include "RenderStage.h"

namespace Zero
{
	class FShadowStage : public FRenderStage
	{
	public:
		PipeStageCreate(FShadowStage)
		FShadowStage() = default;
		virtual ~FShadowStage() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnDraw() override;
	private:
		std::vector<Ref<FRenderTarget2D>> m_ShadowMapRenderTargets;
	};
}