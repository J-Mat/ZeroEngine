#include "RenderTarget.h"
#include "Texture.h"

namespace Zero
{
	FRenderTarget2D::FRenderTarget2D()
	{
		Reset();
	}

	void FRenderTarget2D::Reset()
	{
		m_ColoTexture.resize(7);
		for (uint32_t i = 0; i < 7; ++i)
		{
			m_ColoTexture[i] = FRenderTex2DAttachment();
		}
		m_DepthTexture = FRenderTex2DAttachment();
	}

	void FRenderTargetCube::InitParams()
	{
	}

	FRenderTargetCube::FRenderTargetCube()
	{
		InitParams();
	}

	FRenderTargetCube::FRenderTargetCube(const FRenderTargetCubeDesc& Desc)
		: m_RenderTargetCubeDesc(Desc)
	{
		InitParams();
	}



}
