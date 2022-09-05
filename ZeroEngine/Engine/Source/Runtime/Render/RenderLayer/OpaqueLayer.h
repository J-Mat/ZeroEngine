#pragma once

#include "Core.h"
#include "Core/Framework/Layer.h"


namespace Zero
{
	class FOpaqueLayer : public FLayer
	{
	public:
		FOpaqueLayer() = default;
		virtual ~FOpaqueLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate() override;
		void OnDraw() override;
	private:
	};
}