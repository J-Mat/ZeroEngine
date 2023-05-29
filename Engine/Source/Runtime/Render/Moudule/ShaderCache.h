#pragma once

#include "Core.h"
#include "Render/RHI/GraphicDevice.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RenderConfig.h"
#include "FileWatcher.h"
#include "Render/RHI/Shader/Shader.h"

namespace Zero
{
	DEFINITION_MULTICAST_DELEGATE(FShaderRecompiledEvent, void, uint32_t)

	class FShaderCache : public IPublicSingleton<FShaderCache>
	{
	public:
		FShaderCache();
		Ref<FShader> CreateGraphicShader(const FShaderDesc& ShaderDesc);

		FShaderRecompiledEvent& GetShaderRecompiledEvent() { m_ShaderRecompiledEvent; };

		void OnShaderFileChanged(const std::string& Filename);
		void CheckIfShadersHaveChanged();
	private:
		FFileWatcher m_FileWatcher;
		std::vector<Ref<FShader>> m_AllShaders;
		FShaderRecompiledEvent m_ShaderRecompiledEvent;
	};
}