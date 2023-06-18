#include "TestPostprocessPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/CommandList.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Render/RHI/CommandList.h"

namespace Zero
{
	FTestPostprecessPass::FTestPostprecessPass(uint32_t Width, uint32_t Height)
		: m_Width(Width),
		m_Height(Height)
	{
	}

	void FTestPostprecessPass::AddPass(FRenderGraph& RenderGraph)
	{
		struct FPassData
		{
			FRGTex2DReadOnlyID SrcTex;
			FRGTex2DReadWriteID DstTex;
		};
		auto& Pass = RenderGraph.AddPass<FPassData>(
			"TestPostProcess Pass",
			[=](FPassData& Data, FRenderGraphBuilder& Builder)
			{
				Data.SrcTex = Builder.ReadTexture2D(RGResourceName::GBufferColor);
				{
					FRGTextureDesc ColorDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
						.Format = EResourceFormat::R8G8B8A8_UNORM

					};
					Builder.DeclareTexture2D(RGResourceName::PostProcessBuffer, ColorDesc);
					Data.DstTex = Builder.WriteTexture2D(RGResourceName::PostProcessBuffer);
				}
			},
			[=](const FPassData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				Ref<FComputeRenderItemPool> ComputeRenderItemPool = UWorld::GetCurrentWorld()->GetComputeRenderItemPool();
				Ref<FComputeRenderItem> Item = ComputeRenderItemPool->Request();
				Item->SetPsoID(EComputePsoID::TestPostProcess);
				Item->PreDispatch(CommandListHandle);
				Ref<FShaderParamsGroup> ShaderParamsBinder = Item->GetShaderParamsGroup();
				FTexture2D* DstTexture = Context.GetTexture2D(Data.DstTex.GetResourceID());
				FTexture2D* SrcTexture = Context.GetTexture2D(Data.SrcTex.GetResourceID());
				ShaderParamsBinder->SetTexture2D("SrcTexture", SrcTexture);
				ShaderParamsBinder->SetTexture2D_Uav("DstTexture", DstTexture);
				
				Item->Dispatch(CommandListHandle, m_Width / 8, m_Height / 8, 1);
				
			},
			ERenderPassType::Compute,
			ERGPassFlags::ForceNoCull
		);
	}

	void FTestPostprecessPass::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}
}

