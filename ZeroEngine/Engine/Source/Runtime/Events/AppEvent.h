#pragma once

#include "Event.h"

namespace Zero
{
	class FWindowResizeEvent :public FEvent
	{
	public:
		FWindowResizeEvent(unsigned int InWidth, unsigned int Inheight)
			:Width(InWidth), Height(Inheight)
		{}

		inline unsigned int GetWidth() const { return Width; }
		inline unsigned int GetHeight() const { return Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << Width << ", " << Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);

	private:
		unsigned int Width, Height;
	};

	class SIByL_API FWindowCloseEvent :public FEvent
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class SIByL_API FAppTickEvent :public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class FAppUpdateEvent :public FEvent
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};

	class FAppRenderEvent :public FEvent
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};
}