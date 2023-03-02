#include "DX12ComputeShader.h"
#include "DX12ShaderBinder.h"
#include "ZConfig.h"

namespace Zero
{
	FDX12ComputeShader::FDX12ComputeShader(const FShaderBinderDesc& BinderDesc, const FComputeShaderDesc& Desc)
		: FComputeShader(BinderDesc, Desc)
	{
		std::string FileName = ZConfig::GetAssestsFullPath(m_ComputeShaderDesc.ShaderName).string();
		m_ShaderPass =  CompileShader(Utils::String2WString(FileName), nullptr, m_ComputeShaderDesc.CSEntryPoint, "cs_5_1");
		CreateBinder();
	}

	FDX12ComputeShader::FDX12ComputeShader(const FComputeShaderDesc& Desc)
		: FComputeShader(Desc)
	{
		std::string FileName = ZConfig::GetAssestsFullPath(m_ComputeShaderDesc.ShaderName).string();
		m_ShaderPass = CompileShader(Utils::String2WString(FileName), nullptr, m_ComputeShaderDesc.CSEntryPoint, "cs_5_1");
	}

	void FDX12ComputeShader::CreateBinder()
	{
		m_ShaderBinder = CreateRef<FDX12ShaderBinder>(m_ShaderBinderDesc);
	}
	ComPtr<ID3DBlob> FDX12ComputeShader::CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target)
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
		{
			OutputDebugStringA((char*)Errors->GetBufferPointer());
			return nullptr;
		}

		return ByteCode;
	}
}
