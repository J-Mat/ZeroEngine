#pragma once

#include "../Base/FrameTimer.h"


namespace Zero
{
	class FEvent;
	class FLayer
	{
	public:
		FLayer(const std::string& Iname = "Layer")
			: m_Name(Iname)
		{}
		virtual ~FLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnDraw() = 0;
		virtual void OnImGuiRender() {}
		virtual void OnEvent(FEvent& event) {}
	protected:
		std::string m_Name;
	};
}

