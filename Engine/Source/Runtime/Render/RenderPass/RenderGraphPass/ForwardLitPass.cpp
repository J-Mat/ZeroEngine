#include "ForwardLitPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RenderConfig.h"
#include "Data/Settings/SceneSettings.h"
#include "Render/Moudule/ImageBasedLighting.h"
#include "Render/RHI/Texture.h"
#include "Render/RenderUtils.h"
#include "World/LightManager.h"

namespace Zero
{
	FForwardLitPass::FForwardLitPass(uint32_t Width, uint32_t Height)
		: m_Width(Width),
		m_Height(Height)
	{
	}

	void FForwardLitPass::AddPass(FRenderGraph& RenderGraph)
	{
		RenderGraph.AddPass<void>(
			"ForwardLit Pass",
			[&](FRenderGraphBuilder& Builder)
			{
				{
					FRGTextureDesc DepthDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(1.0f, 0),
						.Format = EResourceFormat::D24_UNORM_S8_UINT
					};
					Builder.DeclareTexture(RGResourceName::DepthStencil, DepthDesc);
					Builder.WriteDepthStencil(RGResourceName::DepthStencil, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc ColorDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM
					};
					Builder.DeclareTexture(RGResourceName::GBufferColor, ColorDesc);
					Builder.WriteRenderTarget(RGResourceName::GBufferColor, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				Builder.SetViewport(m_Width, m_Height);
			},
			[&](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				const std::vector<UDirectLightActor*>& DirectLights = FLightManager::Get().GetDirectLights();
				std::vector<FTexture2D*> ShadowMaps;
				for (uint32_t LightIndex = 0; LightIndex < DirectLights.size(); ++LightIndex)
				{
					FRGTextureID RGTextureID =  RenderGraph.GetTextureID(RGResourceName::ShadowMaps[LightIndex]);
					ShadowMaps.push_back(RenderGraph.GetTexture(RGTextureID));
				}

				FRenderUtils::RenderLayer(ERenderLayer::Light, CommandListHandle);
				FRenderUtils::RenderLayer(ERenderLayer::Opaque, CommandListHandle, 
					[&](Ref<FRenderItem> RenderItem)
					{
						auto& IBLModule = FRenderUtils::GetIBLMoudule();
						RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", IBLModule->GetIrradianceRTCube()->GetColorCubemap().get());
						RenderItem->m_Material->SetTextureCubemap("IBLPrefilterMap", IBLModule->GetPrefilterEnvMapRTCube()->GetColorCubemap().get());
						RenderItem->m_Material->SetTexture2D("_BrdfLUT", FTextureManager::Get().GetLutTexture().get());
						RenderItem->m_Material->SetIBL(true);
						if (ShadowMaps.size() > 0)
						{
							RenderItem->m_Material->SetTexture2DArray("_ShadowMaps", ShadowMaps);
						}
					}
					);
			},
			ERenderPassType::Graphics,
			ERGPassFlags::ForceNoCull
		);
	}

	void FForwardLitPass::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}
}
