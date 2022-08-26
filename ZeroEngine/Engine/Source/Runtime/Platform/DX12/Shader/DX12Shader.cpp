#include "DX12Shader.h"
#include "Utils.h"

namespace Zero
{
	FDX12Shader::FDX12Shader()
	: IShader()
	{
	}
	FDX12Shader::FDX12Shader(std::string FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
	: IShader()
	{
		m_ShaderDesc = Desc;
		m_ShaderBinderDesc = BinderDesc;
		
		m_VSBytecode = CompileShader(Utils::String2WString(FileName), nullptr, "VS", "vs_5_1");
		m_PSBytecode = CompileShader(Utils::String2WString(FileName), nullptr, "PS", "vs_5_1");
	}

	ComPtr<ID3DBlob> FDX12Shader::CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target)
	{
		UINT CompileFlags = 0;
	#if defined(DEBUG) || defined(_DEBUG)  
		CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif
		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> ByteCode = nullptr;
		ComPtr<ID3DBlob> Errors;

		hr = D3DCompileFromFile(
			Filename.c_str(),
			Defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			Entrypoint.c_str(),
			Target.c_str(),
			CompileFlags,
			0,
			&ByteCode,
			&Errors
		);

		if (Errors != nullptr)
			OutputDebugStringA((char*)Errors->GetBufferPointer());

		ThrowIfFailed(hr);

		return ByteCode;
	}
}
