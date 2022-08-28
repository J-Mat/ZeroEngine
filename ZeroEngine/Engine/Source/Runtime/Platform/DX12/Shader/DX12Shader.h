#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RHI/Shader.h"

namespace Zero
{
	class FDX12Shader : public IShader
	{
	public:
		FDX12Shader();
		FDX12Shader(std::string FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
	private:
		ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);
	private:
		ComPtr<ID3DBlob> m_VSBytecode = nullptr;
		ComPtr<ID3DBlob> m_PSBytecode = nullptr;
	};
}