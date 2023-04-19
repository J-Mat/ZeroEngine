#include "SkyboxPass.h"
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



namespace Zero
{
	FSkyboxPass::FSkyboxPass()
	{
	}

	void FSkyboxPass::AddPass(FRenderGraph& RenderGraph)
	{
		RenderGraph.AddPass<void>("Skybox Pass",
			[=](FRenderGraphBuilder& Builder)
			{
				Builder.WriteDepthStencil(RGResourceName::DepthStencil, ERGLoadStoreAccessOp::Preserve_Preserve);
				Builder.WriteRenderTarget(RGResourceName::GBufferColor, ERGLoadStoreAccessOp::Preserve_Preserve);
			},
			[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
				FRenderUtils::FRenderFunc RenderFunc = [&](Ref<FRenderItem> RenderItem)
				{
				};
				FRenderUtils::RenderLayer(ERenderLayer::Skybox, CommandListHandle, RenderFunc);
			},
			ERenderPassType::Graphics,
			ERGPassFlags::ForceNoCull
		);
	}
}
