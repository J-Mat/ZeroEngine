#pragma once

#include "Event.h"

namespace Zero
{
	class FMouseMovedEvent : public FEvent
	{
	public:
		FMouseMovedEvent(int x, int y)
			:MouseX(x), MouseY(y) {}

		inline int GetX() const { return MouseX; }
		inline int GetY() const { return MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << MouseX << ", " << MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		int MouseX, MouseY;
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

	class FMouseButtonDownEvent : public FMouseButtonEvent
	{
	public:
		FMouseButtonDownEvent(int button)
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
		FMouseButtonReleasedEvent(int InButton, int X, int Y)
			: FMouseButtonEvent(InButton)
			, MouseX(X)
			, MouseY(Y)
		{}

		inline int GetX() const { return MouseX; }
		inline int GetY() const { return MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << Button <<  "  " << MouseX << " " << MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	private:
		int MouseX, MouseY;
	};

	class FMouseButtonDoubleClickEvent : public FMouseButtonEvent
	{
	public:
		FMouseButtonDoubleClickEvent(int InButton, int X, int Y)
			: FMouseButtonEvent(InButton)
			, MouseX(X)
			, MouseY(Y)
		{}

		inline int GetX() const { return MouseX; }
		inline int GetY() const { return MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonDoubleClickEvent: " << Button <<  "  " << MouseX << " " << MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonDubleClick)
	private:
		int MouseX, MouseY;
	};
}