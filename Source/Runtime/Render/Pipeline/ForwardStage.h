#pragma once

#include "Core.h"
#include "RenderStage.h"


namespace Zero
{
	class FForwardStage : public FRenderStage
	{
	public:
		PipeStageCreate(FForwardStage)
		FForwardStage() = default;
		virtual ~FForwardStage() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnDraw() override;
	};
}