#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RenderConfig.h"
#include "FileWatcher.h"
#include "Render/RHI/Shader/Shader.h"

namespace Zero
{
	enum EShaderID : uint8_t
	{
		SkyBox,
		ForwardLit,
		DirectLight,
		PointLight,
		IBLIrradiance,
		IBLPrefilter,
		Shadow,
		ShadowMap,
		ShadowDebug,
		DirectLightShadowMap,
	};

	DEFINITION_MULTICAST_DELEGATE(FShaderRecompiledEvent, void, EShaderID)

	class FShaderCache : public IPublicSingleton<FShaderCache>
	{
	public:
		Ref<FShader> CreateShader(EShaderID ShaderID, const FShaderDesc& ShaderDesc);

		FShaderRecompiledEvent& GetShaderRecompiledEvent() { m_ShaderRecompiledEvent; };

		void OnShaderFileChanged(const std::string& Filename);
	private:
		FFileWatcher m_FileWatcher;
		std::map<EShaderID, Ref<FShader>> m_AllShaders;
		FShaderRecompiledEvent m_ShaderRecompiledEvent;
	};
}