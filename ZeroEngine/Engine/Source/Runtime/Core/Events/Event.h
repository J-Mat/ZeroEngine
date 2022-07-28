#pragma once

#include "Core.h"

namespace Zero
{
	enum class EEventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
	};

	enum EEventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4),
	};
#define EVENT_CLASS_TYPE(Type) static  EEventType GetStaticType() {return EEventType::##Type;}\
								virtual EEventType GetEventType() const override {return GetStaticType();}\
								virtual const char* GetName() const override {return #Type;}
#define EVENT_CLASS_CATEGORY(Category) virtual int GetCategoryFlags() const override {return Category;}
	
	class FEvent
	{
		friend class FEventDispatcher;
	public:
		virtual EEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EEventCategory Category)
		{
			return GetCategoryFlags() & Category;
		}
		
		bool Handled = false;
	};


	class FEventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		FEventDispatcher(FEvent& InEvent)
			:Event(InEvent)
		{
		}
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (Event.GetEventType() == T::GetStaticType())
			{
				Event.Handled = func(*(T*)&Event);
				return true;
			}
			return false;
		}
	private:
		FEvent& Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const FEvent& e)
	{
		return os << e.ToString();
	}
}