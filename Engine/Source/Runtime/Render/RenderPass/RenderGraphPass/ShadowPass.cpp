#include "ShadowPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/CommandList.h"
#include "Render/RenderUtils.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/Moudule/ImageBasedLighting.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "World/LightManager.h"



namespace Zero
{
	FShadowPass::FShadowPass(uint32_t Width, uint32_t Height)
		: m_Width(Width)
		, m_Height(Height)
	{
	}

	void FShadowPass::SetupDirectLightShadow(FRenderGraph & RenderGraph)
	{
		uint32_t DirectLightNum = FLightManager::Get().GetMaxDirectLightsNum();
		for (uint32_t DirectLightIndex = 0; DirectLightIndex < DirectLightNum; ++DirectLightIndex)
		{
			RenderGraph.AddPass<void>("Shadow Pass",
				[=](FRenderGraphBuilder& Builder)
				{
					FRGTextureDesc ShadowMapDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(1.0f, 0),
						.Format = EResourceFormat::D24_UNORM_S8_UINT,
					};

					Builder.DeclareTexture(RGResourceName::ShadowMaps[DirectLightIndex], ShadowMapDesc);
					Builder.WriteDepthStencil(RGResourceName::ShadowMaps[DirectLightIndex], ERGLoadStoreAccessOp::Clear_Preserve);
				},
				[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
				{
					FRenderUtils::RenderLayer(ERenderLayer::Shadow, CommandListHandle);
				},
				ERenderPassType::Graphics,
				ERGPassFlags::ForceNoCull
			);
		}
	}

	void FShadowPass::SetupPointLightShadow(FRenderGraph& RenderGraph)
	{
	}

	void FShadowPass::AddPass(FRenderGraph& RenderGraph)
	{
		SetupDirectLightShadow(RenderGraph);
		SetupPointLightShadow(RenderGraph);
	}

	void FShadowPass::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}


}
