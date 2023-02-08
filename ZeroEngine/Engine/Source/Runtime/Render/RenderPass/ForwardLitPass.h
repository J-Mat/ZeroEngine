#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderGraph;
	class FForwardLitPass
	{
	public:
		FForwardLitPass(uint32_t Width, uint32_t Height);
		
		void AddPass(FRenderGraph& RenderGraph);
		
		void OnResize(uint32_t Width, uint32_t Height);
	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};
}