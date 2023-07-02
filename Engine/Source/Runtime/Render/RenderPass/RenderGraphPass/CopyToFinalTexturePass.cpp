#include "CopyToFinalTexturePass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/CommandList.h"


namespace Zero
{
	FCopyToFinalTexturePass::FCopyToFinalTexturePass()
	{
	}

	void FCopyToFinalTexturePass::AddPass(FRenderGraph& RenderGraph, FRGResourceName CopySrcResourceName)
	{
		struct FCopyToBackbufferPassData
		{
			FRGTexture2DCopySrcID Src;
			FRGTexture2DCopyDstID Dst;
		};

		RenderGraph.AddPass<FCopyToBackbufferPassData>("CopyToFinalTexture Pass",
			[=](FCopyToBackbufferPassData& Data, FRenderGraphBuilder& Builder)
			{
				Data.Src = Builder.ReadCopySrcTexture(CopySrcResourceName);
				Data.Dst = Builder.WriteCopyDstTexture(RGResourceName::FinalTexture);
			},
			[=](const FCopyToBackbufferPassData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				Ref<FCommandList> RHICommandList = FGraphic::GetDevice()->GetRHICommandList(CommandListHandle);
				FTexture2D* DstTexture = Context.GetTexture2D(Data.Dst);
				FTexture2D* SrcTexture = Context.GetTexture2D(Data.Src);
				RHICommandList->CopyResource(DstTexture->GetNative(), SrcTexture->GetNative());
				//RHICommandList->TransitionBarrier(DstTexture->GetNative(), EResourceState::Common);
			},
			ERenderPassType::Copy,
			ERGPassFlags::ForceNoCull
		);
	}

}
