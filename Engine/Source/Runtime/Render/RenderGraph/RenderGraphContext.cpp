#include "RenderGraphContext.h"
#include "RenderGraph.h"

namespace Zero
{
	FRenderGraphContext::FRenderGraphContext(FRenderGraph& RenderGraph, FRGPassBase& RGPass)
		: m_RenderGrpah(RenderGraph),
		m_RgPass(RGPass)
	{
	}

	FTexture2D* FRenderGraphContext::GetTexture(FRGTextureID ResID) const
	{
		return m_RenderGrpah.GetTexture(ResID);
	}

	FBuffer* FRenderGraphContext::GetBuffer(FRGBufferID ResID) const
	{
		return m_RenderGrpah.GetBuffer(ResID);
	}
}
