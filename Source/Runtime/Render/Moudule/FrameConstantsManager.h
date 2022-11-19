#pragma once
#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/Shader.h"

namespace Zero
{
	class FFrameConstantsManager : public IPublicSingleton<FFrameConstantsManager>
	{
	public:
		FFrameConstantsManager();
		virtual ~FFrameConstantsManager() = default;
		Ref<IShaderConstantsBuffer> GetShaderConstantBuffer() { return m_ShaderConstantsBuffer; }
	protected:
		Ref<IShaderConstantsBuffer> m_ShaderConstantsBuffer = nullptr;
	};
}