#pragma once

#include "../Base/Base.h"
#include "../Base/Timer.h"

namespace Zero
{
	class FLayer
	{
	public:
		FLayer(const std::string& Iname = "Layer")
			: Name(Iname);
		virtual ~FLayer() = default;
	
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(FTimestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(FEvent& event) {}
	protected:
		std::string Name;
	}
}

