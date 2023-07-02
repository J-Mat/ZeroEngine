#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderGraph;
	class FTaaPass
	{
	public:
		FTaaPass(uint32_t Width, uint32_t Height);

		FRGResourceName AddPass(FRenderGraph& RenderGraph);
		void OnResize(uint32_t Width, uint32_t Height);
	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};
}