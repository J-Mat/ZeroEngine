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
		struct FForwardLitPassData
		{
			FRGTex2DReadOnlyID DirectLightShadowMaps[2];
		};

		RenderGraph.AddPass<FForwardLitPassData>(
			"ForwardLit Pass",
			[=](FForwardLitPassData& Data, FRenderGraphBuilder& Builder)
			{
				{
					FRGTextureDesc DepthDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(1.0f, 0),
						.Format = EResourceFormat::D24_UNORM_S8_UINT
					};
					Builder.DeclareTexture2D(RGResourceName::DepthStencil, DepthDesc);
					Builder.WriteDepthStencil2D(RGResourceName::DepthStencil, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc ColorDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferColor, ColorDesc);
					Builder.WriteRenderTarget2D(RGResourceName::GBufferColor, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				// Shadow Maps
				const std::vector<UDirectLightActor*>& DirectLights = FLightManager::Get().GetDirectLights();
				std::vector<FTexture2D*> ShadowMaps;
				for (size_t LightIndex = 0; LightIndex < DirectLights.size(); ++LightIndex)
				{
					FRGTex2DReadOnlyID RGTextureReadOnlyID =  Builder.ReadTexture2D(RGResourceName::DirectLightShadowMaps[LightIndex]);
					Data.DirectLightShadowMaps[LightIndex] = RGTextureReadOnlyID;
				}

				Builder.SetViewport(m_Width, m_Height);
			},
			[=](const FForwardLitPassData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				const std::vector<UDirectLightActor*>& DirectLights = FLightManager::Get().GetDirectLights();
				std::vector<FTexture2D*> ShadowMaps;
				for (size_t LightIndex = 0; LightIndex < DirectLights.size(); ++LightIndex)
				{
					FTexture2D* Texture = Context.GetTexture2D(Data.DirectLightShadowMaps[LightIndex].GetResourceID());
					ShadowMaps.push_back(Texture);
				}


				{
					FRenderSettings LightRenderSettings =
					{
						.RenderLayer = ERenderLayer::Light,
						.PiplineStateMode = EPiplineStateMode::Dependent,
					};
					FRenderUtils::RenderLayer(LightRenderSettings, CommandListHandle);
				}
				
				{
					FRenderSettings ForwardRenderSettings =
					{
						.RenderLayer = ERenderLayer::Opaque,
						.PiplineStateMode = EPiplineStateMode::Dependent,
					};
					FRenderUtils::RenderLayer(ForwardRenderSettings, CommandListHandle,
						[&](Ref<FRenderItem> RenderItem)
						{
							auto& IBLModule = FRenderUtils::GetIBLMoudule();
							RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", IBLModule->GetIrradianceRTCube()->GetColorTexCube());
							RenderItem->m_Material->SetTextureCubemap("IBLPrefilterMap", IBLModule->GetPrefilterEnvMapRTCube()->GetColorTexCube());
							RenderItem->m_Material->SetTexture2D("_BrdfLUT", FTextureManager::Get().GetLutTexture().get());
							RenderItem->m_Material->SetIBL(true);
							uint32_t EmptyLightsNum = FLightManager::Get().GetMaxDirectLightsNum() - uint32_t(ShadowMaps.size());
							while (EmptyLightsNum--)
							{
								ShadowMaps.push_back(FTextureManager::Get().GetDefaultTexture().get());
							}
							RenderItem->m_Material->SetTexture2DArray("_gShadowMaps", ShadowMaps);
						}
					);
				}
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
