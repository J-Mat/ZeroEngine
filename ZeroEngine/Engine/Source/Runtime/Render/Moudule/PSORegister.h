#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"

namespace Zero
{
	class FPSORegister : public IPublicSingleton<FPSORegister>
	{
	public:
		void RegisterDefaultPSO();
		void RegisterSamplePSO();
		void RegisterSkyboxPSO();
	};
}