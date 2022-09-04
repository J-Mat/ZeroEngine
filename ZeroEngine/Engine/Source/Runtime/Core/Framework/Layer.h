#pragma once

#include "../Base/FrameTimer.h"


namespace Zero
{
	class FEvent;
	class FLayer
	{
	public:
		FLayer(const std::string& Iname = "Layer", uint32_t DeviceIndex = 0)
			: m_Name(Iname)
			, m_DeviceIndex(DeviceIndex)
		{}
		virtual ~FLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnDraw() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(FEvent& event) {}
	protected:
		uint32_t m_DeviceIndex = 0;
		std::string m_Name;
	};
}

