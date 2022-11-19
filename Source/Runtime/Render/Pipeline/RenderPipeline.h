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

		void PushLayer(Ref<FRenderStage> Layer);
		void Run();

		std::vector<Ref<FRenderStage>>::iterator begin() { return m_Stages.begin(); }
		std::vector<Ref<FRenderStage>>::iterator end() { return m_Stages.end(); }

	private:
		std::vector<Ref<FRenderStage>> m_Stages;
	};
}