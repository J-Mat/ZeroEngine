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
		Ref<IDevice> GetDevice() { return m_Device; }
		void SetDevice(Ref<IDevice> Device) { m_Device = Device; }
	private:
		Ref<IDevice> m_Device;
	};
}