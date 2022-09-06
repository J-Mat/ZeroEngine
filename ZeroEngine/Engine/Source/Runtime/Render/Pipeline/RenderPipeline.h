#pragma once

#include "Core.h"
#include "RenderStage.h"


namespace Zero
{
	class FRenderPipeline
	{
	public:
		FRenderPipeline() = default;
		~FRenderPipeline() = default;

		void PushLayer(FPipelineStage* Layer);

		std::vector<FPipelineStage*>::iterator begin() { return m_Stages.begin(); }
		std::vector<FPipelineStage*>::iterator end() { return m_Stages.end(); }

	private:
		std::vector<FPipelineStage*> m_Stages;
	};
}