#pragma once
#include "Core.h"
#include "ForwardLitPass.h"
#include "../RenderGraph/RenderGraphResourcePool.h"

namespace Zero
{
	class FRenderer
	{
	public:
		FRenderer() = delete;
		FRenderer(uint32_t Width, uint32_t Height);
		~FRenderer();
	
		void SetupShadow();

		void OnResize(uint32_t Width, uint32_t Height);
		void OnUpdate();
		void OnDraw();
	private:
		FRGResourcePool m_RGResourcePool;
		uint32_t m_Width;
		uint32_t m_Height;
		
		FForwardLitPass m_ForwardLitPass;
	};
};
