#include "TaaPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/CommandList.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Render/RHI/CommandList.h"
#include "Render/RenderUtils.h"

namespace Zero
{
	FTaaPass::FTaaPass(uint32_t Width, uint32_t Height)
		: m_Width(Width),
		m_Height(Height)
	{
	}

	FRGResourceName FTaaPass::AddPass(FRenderGraph& RenderGraph)
	{
		struct FTaaPassData
		{
			FRGTex2DReadOnlyID SrcTex;
			FRGTex2DReadWriteID DstTex;
		};
		auto& Pass = RenderGraph.AddPass<FTaaPassData>(
			"TestPostProcess Pass",
			[=](FTaaPassData& Data, FRenderGraphBuilder& Builder)
			{	
			},
			[=](const FTaaPassData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
			{
			},
			ERenderPassType::Graphics,
			ERGPassFlags::ForceNoCull
		);
		
		return RGResourceName::TaaOutput;
	}

	void FTaaPass::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}
}

