#pragma once
#include <ZeroEngine.h>
#include "World/Base/PropertyObject.h"

#define CONSTRUCT_NUM_WIDGET_MARCRO(type, Type, Number) \



namespace Zero
{
	namespace NumberWidget
	{
		
		bool ConstructWidget(
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
				}
			}
		}
		
		template<class int, int Num>
		bool ConstructNumWidget(UProperty* Property)
		{
			const FClassInfoCollection&  ClassInfoCollection = Property->GetClassCollection();
			
			T* Data = (T*)Property->GetData<T>();
		}
		
		bool ConstructFloat3Widget(UProperty* Property)
		{
			
			ConstructNumWidget<>(Property, );
		}
	}
}