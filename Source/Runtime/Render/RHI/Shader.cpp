#include "Shader.h"
namespace Zero
{
	IShader::IShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
		: m_ShaderBinderDesc(BinderDesc)
		, m_ShaderDesc(Desc)
	{
	}
}
