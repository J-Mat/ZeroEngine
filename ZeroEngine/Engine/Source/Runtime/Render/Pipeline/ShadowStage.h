#pragma once

#include "Core.h"
#include "RenderStage.h"

namespace Zero
{
	class FRenderItem;
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
		void RegisterDebugShadowMap();
		void RegisterShadowMap();
		void RenderShadowMapDebug();
		void RenderShadowMap();
	private:
		Ref<FRenderItem> m_ShadowMapDebugItem;
		std::vector<Ref<FRenderTarget2D>> m_ShadowMapRenderTargets;
		Ref<FRenderTarget2D> m_ShadowMapDebugRenderTarget;
	};
}