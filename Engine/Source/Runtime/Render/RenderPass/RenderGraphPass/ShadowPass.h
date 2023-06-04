#pragma once
#include "Core.h"
#include "Render/RHI/RenderItem.h"

namespace Zero
{
	class FRenderGraph;
	class FShadowPass
	{
	public:
		FShadowPass(uint32_t Width, uint32_t Height);

		void AddPass(FRenderGraph& RenderGraph);
		void OnResize(uint32_t Width, uint32_t Height);
	private:
		void SetupDirectLightShadow(FRenderGraph& RenderGraph);
		void SetupPointLightShadow(FRenderGraph& RenderGraph);
		void ComputeShaderTest(FRenderGraph& RenderGraph);
	private:
		Ref<FRenderItem> m_ShadowMapDebugItems[2];
		uint32_t m_Width;
		uint32_t m_Height;
	};
}