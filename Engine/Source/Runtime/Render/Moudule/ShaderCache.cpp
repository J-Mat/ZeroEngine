#include "ShaderCache.h"
#include "Render/RendererAPI.h"
#include "ZConfig.h"

namespace Zero
{
	FShaderCache::FShaderCache()
	{
        m_FileWatcher.AddPathToWatch(ZConfig::ShaderDir.string());
        m_FileWatcher.GetFileModifiedEvent().AddFunction<FShaderCache>(this, &FShaderCache::OnShaderFileChanged);
	}

	Ref<FShader> FShaderCache::CreateShader(EShaderID ShaderID, const FShaderDesc& ShaderDesc)
    {
        auto Shader =  FRenderer::GetDevice()->CreateShader(ShaderDesc);
        m_AllShaders.insert({ShaderID, Shader});
        return Shader;
    }

    void FShaderCache::OnShaderFileChanged(const std::string& Filename)
    {
        for (auto [ShaderID, Shader] : m_AllShaders)
        {
            if (Shader->GetAllIncludeFiles().contains(Filename))
            {
                auto Desc = Shader->GetDesc();
                CreateShader(ShaderID, Desc);
            }
           
        }
    }

	void FShaderCache::CheckIfShadersHaveChanged()
	{
        m_FileWatcher.CheckWatchedFiles();
	}

}
