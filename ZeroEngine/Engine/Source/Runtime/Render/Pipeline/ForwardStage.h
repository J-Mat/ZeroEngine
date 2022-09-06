#pragma once

#include "Core.h"
#include "Core/Framework/Layer.h"
#include "ForwardStage.h"


namespace Zero
{
	class FForwardStage : public FLayer
	{
	public:
		FForwardStage() = default;
		virtual ~FForwardStage() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnDraw() override;
	private:
	};
}