#pragma once
#include "Core.h"
#include "./RenderPass/ForwardLitPass.h"
#include "Render/RenderGraph/RenderGraphResourcePool.h"

namespace Zero
{
	class FRenderGraph;
	class FDAGRender
	{
	public:
		FDAGRender() = default;
		FDAGRender(uint32_t Width, uint32_t Height);
		~FDAGRender();
	
		void SetupShadow();

		void CreateSizeDependentResources();
		void OnResize(uint32_t Width, uint32_t Height);
		void OnUpdate();
		void OnDraw();

		FTexture2D* GetFinalTexture()
		{
			return m_FinalTexture.get();
		}
	private:
		Scope<FTexture2D> m_FinalTexture;
		FRGResourcePool m_RGResourcePool;
		uint32_t m_Width;
		uint32_t m_Height;
		
		FForwardLitPass m_ForwardLitPass;
	};
};
