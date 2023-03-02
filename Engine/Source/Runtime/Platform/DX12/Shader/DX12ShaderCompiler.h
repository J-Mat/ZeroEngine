#pragma once
#include "Core.h"
#include "Platform/DX12/Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "Render/RHI/Shader/Shader.h"

namespace Zero
{
	struct FShaderCompileOutput
	{
		Ref<FShader> Shader;
		std::vector<std::string> IncludeFiles{};
	};

	class FDX12ShaderCompiler
	{
	public:
		FDX12ShaderCompiler();
		ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);
		bool CompileShader(EShaderStage ShaderStage, FShaderDesc const& Desc, FShaderCompileOutput& Output);
		ID3D12ShaderReflection* GetShaderReflectionPtr(EShaderStage ShaderStage, Ref<FShader> Shader);
	};
}