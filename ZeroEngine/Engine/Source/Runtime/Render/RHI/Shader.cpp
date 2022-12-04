#include "Shader.h"
namespace Zero
{
	FShader::FShader(const std::string ShaderID, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
		:
		m_ShaderBinderDesc(BinderDesc)
		, m_ShaderDesc(Desc)
	{
	}

	FShader::FShader(const FShaderDesc& Desc)
	: m_ShaderDesc(Desc)
	{
	}

	void FShaderDefines::SetDefine(const std::string& Name, const std::string& Definition)
	{
		DefinesMap.insert_or_assign(Name, Definition);
	}

	bool FShaderDefines::operator==(const FShaderDefines& Other) const
	{
		if (DefinesMap.size() != Other.DefinesMap.size())
		{
			return false;
		}
		for (const auto& Pair : DefinesMap)
		{
			const std::string Key = Pair.first;
			const std::string Value = Pair.second;

			auto Iter = Other.DefinesMap.find(Key);
			if (Iter == Other.DefinesMap.end() || Iter->second != Value)
			{
				return false;
			}
		}
		return true;
	}
}
