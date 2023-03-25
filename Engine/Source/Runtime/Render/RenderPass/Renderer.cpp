#include "Renderer.h"
#include "Render/RenderGraph/RenderGraph.h"

namespace Zero
{
	FRenderer::FRenderer(uint32_t Width, uint32_t Height)
		: m_Width(Width), 
		m_Height(Height),
		m_ForwardLitPass(Width, Height)
	{
	}

	FRenderer::~FRenderer()
	{
	}

	void FRenderer::SetupShadow()
	{
	}

	void FRenderer::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}

	void FRenderer::OnUpdate()
	{
		SetupShadow();
	}

	void FRenderer::OnDraw()
	{
		FRenderGraph RenderGraph(m_RGResourcePool);
		
		m_ForwardLitPass.AddPass(RenderGraph);

	
		RenderGraph.Build();
	}
}

