#pragma once
#include <ZeroEngine.h>
#include "World/Base/PropertyObject.h"
#include "World/Base/ClassInfoCollection.h"

#define CONSTRUCT_NUM_WIDGET_MARCRO(type, Type, Number) \
ConstructNumberWidget(Property,\
[&]()->bool \
{ \
	return ImGui::Drag##Type##Number(Property->GetName().c_str(), (type*)Property->GetData<type>()); \
}, \
[&](int32_t Min, int32_t Max, int32_t Step)->bool {\
	 return ImGui::Drag##Type##Number(Property->GetName().c_str(), (type*)Property->GetData<type>(), Step, Min, Max); \
}, \
[&]()->bool \
{ \
	return ImGui::Input##Type##Number(Property->GetName().c_str(), (type*)Property->GetData<type>()); \
}\
)

namespace Zero
{
	namespace NumberWidget
	{
		
		bool ConstructNumberWidget(
			UProperty* Property,
			std::function<bool()> DraggableFunc,
			std::function<bool(int, int, int)> DraggableMaxMinFunc,
			std::function<bool()> InputFunc)
		{
			FClassInfoCollection&  ClassInfoCollection = Property->GetClassCollection();
			if (ClassInfoCollection.ExitField("Draggable"))
			{
				if (ClassInfoCollection.ExitField("Slide"))
				{
					return DraggableFunc();
				}
				else
				{
					int32_t Max = -1;
					int32_t Min = -1;
					int32_t Step = 1;
					std::string MaxValue = "";
					std::string MinValue = "";
					std::string StepValue = "";
					if (ClassInfoCollection.FindMetas("Max", MaxValue))
					{
						Max = atoi(MaxValue.c_str());
					}
					if (ClassInfoCollection.FindMetas("Min", MinValue))
					{
						Min = atoi(MinValue.c_str());
					}
					if (ClassInfoCollection.FindMetas("Step",StepValue))
					{
						Step = atoi(StepValue.c_str());
					}
					return DraggableMaxMinFunc(Max, Min, Step);
				}
			}
			else
			{
				return InputFunc();
			}
		}
		
		bool ConstructFloatWidget(UProperty* Property)
		{
			return CONSTRUCT_NUM_WIDGET_MARCRO(float, Float, );
		}

		bool ConstructFloat2Widget(UProperty* Property)
		{
			return CONSTRUCT_NUM_WIDGET_MARCRO(float, Float, 2);
		}
		
		bool ConstructFloat3Widget(UProperty* Property)
		{
			return CONSTRUCT_NUM_WIDGET_MARCRO(float, Float, 3);
		}

		bool ConstructFloat4Widget(UProperty* Property)
		{
			return CONSTRUCT_NUM_WIDGET_MARCRO(float, Float, 4);
		}
	}
}