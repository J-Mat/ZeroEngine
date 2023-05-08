#include "RenderGraphContext.h"
#include "RenderGraph.h"

namespace Zero
{
	FRenderGraphContext::FRenderGraphContext(FRenderGraph& RenderGraph, FRGPassBase& RGPass)
		: m_RenderGrpah(RenderGraph),
		m_RgPass(RGPass)
	{
	}

	FTexture2D* FRenderGraphContext::GetTexture2D(FRGTexture2DID ResID) const
	{
		return m_RenderGrpah.GetTexture2D(ResID);
	}

	FTextureCube* FRenderGraphContext::GetTextureCube(FRGTextureCubeID ResID) const
	{
		return m_RenderGrpah.GetTextureCube(ResID);
	}

	FBuffer* FRenderGraphContext::GetBuffer(FRGBufferID ResID) const
	{
		return m_RenderGrpah.GetBuffer(ResID);
	}
}
