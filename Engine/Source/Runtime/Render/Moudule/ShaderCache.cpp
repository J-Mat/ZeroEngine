#include "ShaderCache.h"
#include "Render/RendererAPI.h"
#include "ZConfig.h"
#include "PSOCache.h"

namespace Zero
{
	FShaderCache::FShaderCache()
	{
        m_AllShaders.resize(EShaderID::ShaderCount);
        m_FileWatcher.AddPathToWatch(ZConfig::ShaderDir.string());
        m_FileWatcher.GetFileModifiedEvent().AddFunction<FShaderCache>(this, &FShaderCache::OnShaderFileChanged);
	}

	Zero::Ref<Zero::FShader> FShaderCache::CreateShader(const FShaderDesc& ShaderDesc)
    {
        auto Shader =  FGraphic::GetDevice()->CreateShader(ShaderDesc);
        m_AllShaders[ShaderDesc.ShaderID] = Shader;
        return Shader;
    }

    void FShaderCache::OnShaderFileChanged(const std::string& Filename)
    {
        for (size_t ShaderID = 0; ShaderID < m_AllShaders.size(); ++ShaderID)
        {
            if (!m_AllShaders[ShaderID])
            {
                continue;
            }
            auto Shader = m_AllShaders[ShaderID];
            if (Shader->GetAllIncludeFiles().contains(Filename))
            {
                auto Desc = Shader->GetDesc();
                auto Shader = CreateShader(Desc);
                FPSOCache::Get().OnReCreatePso(Shader);
            }
        }
    }

	void FShaderCache::CheckIfShadersHaveChanged()
	{
        m_FileWatcher.CheckWatchedFiles();
	}

}
