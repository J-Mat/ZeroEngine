#pragma once

#include "Event.h"
namespace Zero
{
	class  FKeyEvent : public FEvent
	{
	public:
		inline int GetKeyCode() const { return KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		FKeyEvent(int InKeycode)
			:KeyCode(InKeycode) {}

		int KeyCode;
	};

	class FKeyPressedEvent : public FKeyEvent
	{
	public:
		FKeyPressedEvent(int InKeycode, int InRepeatCount)
			:FKeyEvent(InKeycode), RepeatCount(InRepeatCount) {}

		inline int GetRepeatCount() const { return RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << KeyCode << " (" << RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int RepeatCount;
	};


	class FKeyReleasedEvent :public FKeyEvent
	{
	public:
		FKeyReleasedEvent(int InKeycode)
			:FKeyEvent(InKeycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased);
	};


	class FKeyTypedEvent :public FKeyEvent
	{
	public:
		FKeyTypedEvent(unsigned int InKeycode)
			:FKeyEvent(InKeycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped);
	};
}