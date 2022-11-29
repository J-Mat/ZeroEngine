#include "Shader.h"
namespace Zero
{
	IShader::IShader(const std::string ShaderID, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
		:
		m_ShaderID(ShaderID)
		,m_ShaderBinderDesc(BinderDesc)
		, m_ShaderDesc(Desc)
	{
	}
}
