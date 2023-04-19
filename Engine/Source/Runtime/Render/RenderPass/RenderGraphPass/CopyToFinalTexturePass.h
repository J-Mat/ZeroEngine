#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderGraph;
	class FCopyToFinalTexturePass
	{
	public:
		FCopyToFinalTexturePass();
		
		void AddPass(FRenderGraph& RenderGraph);
	};
}