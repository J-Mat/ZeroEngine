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

		void PushLayer(FRenderStage* Layer);

		std::vector<FRenderStage*>::iterator begin() { return m_Stages.begin(); }
		std::vector<FRenderStage*>::iterator end() { return m_Stages.end(); }

	private:
		std::vector<FRenderStage*> m_Stages;
	};
}