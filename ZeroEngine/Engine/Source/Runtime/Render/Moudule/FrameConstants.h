#pragma once
#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/Shader.h"

namespace Zero
{
	class IDevice;
	class FFrameConstants
	{
	public:
		FFrameConstants(IDevice* Device);
		virtual ~FFrameConstants() = default;
		Ref<IShaderConstantsBuffer> GetShaderConstantBuffer() { return m_ShaderConstantsBuffer; }
	protected:
		Ref<IShaderConstantsBuffer> m_ShaderConstantsBuffer = nullptr;
	};
}