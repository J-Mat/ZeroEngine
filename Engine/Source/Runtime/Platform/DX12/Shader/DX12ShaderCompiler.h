#pragma once
#include "Core.h"
#include "Platform/DX12/Common/DX12Header.h"
#include "Core/Base/PublicSingleton.h"
#include "DX12Shader.h"

namespace Zero
{
	struct FShaderCompileOutput
	{
		FDX12Shader* Shader = nullptr;
		std::vector<std::string> IncludeFiles{};
		ComPtr<ID3D12ShaderReflection> ShaderReflection = nullptr;
	};

	class FDX12ShaderCompiler : public IPublicSingleton<FDX12ShaderCompiler>
	{
	public:
		FDX12ShaderCompiler() = default;
		void Init();
		ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);
		bool CompileShader(EShaderStage ShaderStage, FShaderDesc const& Desc, FShaderCompileOutput& Output);
	};
}