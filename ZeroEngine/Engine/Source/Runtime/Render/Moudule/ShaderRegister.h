#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"

namespace Zero
{
	class FShaderRegister : public IPublicSingleton<FShaderRegister>
	{
	public:
		void RegisterDefaultShader(IDevice* Device);
	};
}