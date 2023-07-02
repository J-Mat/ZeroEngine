#include "BasePass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RenderUtils.h"
#include "Render/Moudule/Material.h"
#include "Render/Moudule/ImageBasedLighting.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "World/LightManager.h"
#include "Render/Moudule/PSOCache.h"
#include "Render/Moudule/SceneCapture.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Render/RHI/CommandList.h"


namespace Zero
{
	FBasePass::FBasePass(uint32_t Width, uint32_t Height)
	{

	}

	void FBasePass::AddPass(FRenderGraph& RenderGraph)
	{
		struct FBasePassData
		{
			FRGTex2DDepthStencilID DepthStencilID;
			FRGTex2DRenderTargetID BaseColorID;
			FRGTex2DRenderTargetID NormalID;
			FRGTex2DRenderTargetID WorldPosID;
			FRGTex2DRenderTargetID ORMID;
			FRGTex2DRenderTargetID VelocityID;
			FRGTex2DRenderTargetID EmissiveID;
		};
		std::string PassName = "BasePass ";
		RenderGraph.AddPass<FBasePassData>(
			PassName.c_str(),
			[=](FBasePassData& Data, FRenderGraphBuilder& Builder)
			{
				{
					FRGTextureDesc DepthDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(1.0f, 0),
						.Format = EResourceFormat::D24_UNORM_S8_UINT
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferDepthStencil, DepthDesc);
					Data.DepthStencilID =  Builder.WriteDepthStencil2D(RGResourceName::GBufferDepthStencil, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc ColorDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferBaseColor, ColorDesc);
					Data.BaseColorID = Builder.WriteRenderTarget2D(RGResourceName::GBufferBaseColor, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc NormalDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_SNORM
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferNormal, NormalDesc);
					Data.NormalID = Builder.WriteRenderTarget2D(RGResourceName::GBufferNormal, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc WorldPosDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R32G32B32A32_FLOAT
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferWorldPos, WorldPosDesc);
					Data.WorldPosID = Builder.WriteRenderTarget2D(RGResourceName::GBufferWorldPos, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc ORMDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferORM, ORMDesc);
					Data.ORMID = Builder.WriteRenderTarget2D(RGResourceName::GBufferORM, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc VelocityDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R16G16_FLOAT
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferVelocity, VelocityDesc);
					Data.VelocityID = Builder.WriteRenderTarget2D(RGResourceName::GBufferVelocity, ERGLoadStoreAccessOp::Clear_Preserve);
				}

				{
					FRGTextureDesc EmissiveDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM
					};
					Builder.DeclareTexture2D(RGResourceName::GBufferEmissive, EmissiveDesc);
 					Data.EmissiveID = Builder.WriteRenderTarget2D(RGResourceName::GBufferEmissive, ERGLoadStoreAccessOp::Clear_Preserve);
				} 

				Builder.SetViewport(m_Width, m_Height);
			},
			[=](const FBasePassData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				{
					FRenderParams ForwardRenderSettings =
					{
						.RenderLayer = ERenderLayer::Opaque,
						.PiplineStateMode = EPiplineStateMode::Dependent,
					};
					FRenderUtils::RenderLayer(ForwardRenderSettings, CommandListHandle);
				}
				Ref<FCommandList> CommandList = FGraphic::GetDevice()->GetRHICommandList(CommandListHandle);
				
				CommandList->TransitionBarrier(Context.GetTexture2D(Data.BaseColorID.GetResourceID())->GetNative(), EResourceState::Common);
			},
			ERenderPassType::Graphics,
			ERGPassFlags::ForceNoCull
		);
	}

	void FBasePass::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}

}
