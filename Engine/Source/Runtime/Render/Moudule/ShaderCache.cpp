#include "ShaderCache.h"
#include "Render/RendererAPI.h"

namespace Zero
{
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
                Shader->Compile();
            }
        }
    }
}
