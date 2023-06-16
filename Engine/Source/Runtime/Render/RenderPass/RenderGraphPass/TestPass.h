#pragma once
#include "Core.h"

namespace Zero
{
	class FRenderGraph;
	class FTestPass
	{
	public:
		FTestPass();

		void AddPass(FRenderGraph& RenderGraph);
	};
}