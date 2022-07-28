#pragma once

#include "Event.h"

namespace Zero
{
	class FMouseMovedEvent : public FEvent
	{
	public:
		FMouseMovedEvent(float x, float y)
			:MouseX(x), MouseY(y) {}

		inline float GetX() const { return MouseX; }
		inline float GetY() const { return MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << MouseX << ", " << MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float MouseX, MouseY;
	};

	class FMouseScrolledEvent : public FEvent
	{
	public:
		FMouseScrolledEvent(float InXOffset, float InYOffset)
			:XOffset(InXOffset), YOffset(InYOffset) {}

		inline float GetXOffset() const { return XOffset; }
		inline float GetYOffset() const { return YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float XOffset, YOffset;
	};

	class FMouseButtonEvent : public FEvent
	{
	public:
		inline int GetMouseButton() const { return Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		FMouseButtonEvent(int InButton)
			:Button(InButton){}

		int Button;
	};

	class FMouseButtonPressedEvent : public FMouseButtonEvent
	{
	public:
		FMouseButtonPressedEvent(int button)
			: FMouseButtonEvent(button) {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class FMouseButtonReleasedEvent : public FMouseButtonEvent
	{
	public:
		FMouseButtonReleasedEvent(int InButton)
			: FMouseButtonEvent(InButton) {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}