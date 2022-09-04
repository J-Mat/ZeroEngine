#pragma once

#include "Core.h"
#include "Base/CoreObject.h"
#include "Render/RHI/GraphicDevice.h"

namespace Zero
{
	class UWorld : public UCoreObject
	{
	public:
		UWorld()
		{
		}
	private:
		Ref<IDevice> m_Device;
	};
}