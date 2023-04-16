#include "SkyboxPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/CommandList.h"



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
			},
			[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
			},
			ERenderPassType::Graphics,
			ERGPassFlags::ForceNoCull
		);
	}
}
