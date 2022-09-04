#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	class UComponent : public UCoreObject
	{
		UComponent(uint32_t DeviceIndex) 
			: UCoreObject(DeviceIndex)
		{
		}
	};
}