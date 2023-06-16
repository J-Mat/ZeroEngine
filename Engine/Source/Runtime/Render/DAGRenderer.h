#pragma once
#include "Core.h"
#include "RenderPass/RenderGraphPass/ForwardLitPass.h"
#include "RenderPass/RenderGraphPass/CopyToFinalTexturePass.h"
#include "RenderPass/RenderGraphPass/SkyboxPass.h"
#include "RenderPass/RenderGraphPass/ShadowPass.h"
#include "RenderPass/RenderGraphPass/TestPass.h"
#include "Render/RenderGraph/RenderGraphResourcePool.h"

namespace Zero
{
	class FRenderGraph;
	class FDAGRender
	{
	public:
		FDAGRender() = default;
		FDAGRender(uint32_t Width, uint32_t Height);
		~FDAGRender();
	
		void SetupShadow();

		void CreateSizeDependentResources();
		void OnResize(uint32_t Width, uint32_t Height);
		void OnUpdate();
		void SetupEnvironmentMap(FRenderGraph& Rg);
		void ImportResource(FRenderGraph& RenderGraph);
		void OnDraw();

		Ref<FTexture2D> GetFinalTexture()
		{
			return m_FinalTexture;
		}
		
		std::vector<Ref<FTexture2D>>& GetDirectLightShadowMapDebugs()
		{
			return m_DirectLightShadowMapDebugs;
		}

	private:
		Ref<FTexture2D> m_FinalTexture;
		std::vector<Ref<FTexture2D>> m_DirectLightShadowMapDebugs;
		FRGResourcePool m_RGResourcePool;
		uint32_t m_Width;
		uint32_t m_Height;
		
		FForwardLitPass m_ForwardLitPass;
		FSkyboxPass m_SkyboxPass;
		FShadowPass m_ShadowPass;
		FTestPass m_TestPass;
		FCopyToFinalTexturePass m_CopyToFinalTexturePass;
	};
};
