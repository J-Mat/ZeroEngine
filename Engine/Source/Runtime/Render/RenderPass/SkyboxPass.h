#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderGraph;
	class FSkyboxPass
	{
	public:
		FSkyboxPass();
		
		void AddPass(FRenderGraph& RenderGraph);
	};
}