#include "RenderTarget.h"
#include "Texture.h"

namespace Zero
{
	FRenderTarget2D::FRenderTarget2D()
	{
		Reset();
	}

	FRenderTargetCube::FRenderTargetCube(const FRenderTargetCubeDesc& Desc)
		: m_Size(Desc.Size)
		, m_bRenderDepth(Desc.bRenderDepth)
	{
		ZMath::vec3 Eye = {0.0f, 0.0f, 0.0f};
		ZMath::vec3 Targets[6] =
		{
			Eye + ZMath::vec3(1.0f,  0.0f,  0.0f), // +X 
			Eye + ZMath::vec3(-1.0f, 0.0f,  0.0f), // -X 
			Eye + ZMath::vec3(0.0f,  1.0f,  0.0f), // +Y 
			Eye + ZMath::vec3(0.0f,  -1.0f, 0.0f), // -Y 
			Eye + ZMath::vec3(0.0f,  0.0f,  1.0f), // +Z 
			Eye + ZMath::vec3(0.0f,  0.0f, -1.0f)  // -Z 
		};

		ZMath::vec3 Ups[6] =
		{
			{0.0f, 1.0f, 0.0f},  // +X 
			{0.0f, 1.0f, 0.0f},  // -X 
			{0.0f, 0.0f, -1.0f}, // +Y 
			{0.0f, 0.0f, +1.0f}, // -Y 
			{0.0f, 1.0f, 0.0f},	 // +Z 
			{0.0f, 1.0f, 0.0f}	 // -Z 
		};


		for (int i = 0; i < 6; ++i)
		{
			SceneViews[i].EyePos = Eye;
			SceneViews[i].View = ZMath::lookAtLH(Eye, Targets[i], Ups[i]);

			float Fov = ZMath::half_pi<float>();
			float AspectRatio = 1.0f; //Square
			SceneViews[i].Proj = ZMath::perspectiveLH(Fov, AspectRatio, 0.1f, 10.0f);

			SceneViews[i].ProjView = SceneViews[i].Proj * SceneViews[i].View;

			SceneViews[i].Near = 0.0f;
			SceneViews[i].Far = 10.0f;
		}
	}
}
