#include "RenderPipeline.h"

namespace Zero
{
	void FRenderPipeline::PushLayer(Ref<FRenderStage> Layer)
	{
		m_Stages.push_back(Layer);
		Layer->OnAttach();
	}
	void FRenderPipeline::Run()
	{
		for (auto Stage : m_Stages)
		{
			Stage->OnDraw();
		}
	}
}
