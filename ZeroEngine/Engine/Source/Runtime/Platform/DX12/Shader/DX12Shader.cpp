#include "DX12Shader.h"
#include "Utils.h"
#include "Platform/DX12/DX12Device.h"

namespace Zero
{
	DXGI_FORMAT ShaderDataTypeToDXGIFormat(EShaderDataType Type)
	{
		switch (Type)
		{
		case Zero::EShaderDataType::None:	return DXGI_FORMAT_R8_TYPELESS;
		case Zero::EShaderDataType::Float:	return DXGI_FORMAT_R32_FLOAT;
		case Zero::EShaderDataType::Float2:	return DXGI_FORMAT_R32G32_FLOAT;
		case Zero::EShaderDataType::Float3:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case Zero::EShaderDataType::Float4:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Zero::EShaderDataType::Mat3:	return DXGI_FORMAT_R8_TYPELESS;
		case Zero::EShaderDataType::Mat4:	return DXGI_FORMAT_R8_TYPELESS;
		case Zero::EShaderDataType::Int:	return DXGI_FORMAT_R16_SINT;
		case Zero::EShaderDataType::Int2:	return DXGI_FORMAT_R8G8_SINT;
		case Zero::EShaderDataType::Int3:	return DXGI_FORMAT_R32G32B32_SINT;
		case Zero::EShaderDataType::Int4:	return DXGI_FORMAT_R8G8B8A8_SINT;
		case Zero::EShaderDataType::Bool:	return DXGI_FORMAT_R8_TYPELESS;
		case Zero::EShaderDataType::RGB:	return DXGI_FORMAT_R32G32B32_FLOAT;
		case Zero::EShaderDataType::RGBA:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:return DXGI_FORMAT_R8_TYPELESS;
		}
	}

	FDX12Shader::FDX12Shader()
	: IShader()
	{
	}
	FDX12Shader::FDX12Shader(FDX12Device& Device, std::string FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
	: m_Device(Device)
	, IShader()
	{
		m_ShaderDesc = Desc;
		m_ShaderBinderDesc = BinderDesc;
		
		m_VSBytecode = CompileShader(Utils::String2WString(FileName), nullptr, "VS", "vs_5_1");
		m_PSBytecode = CompileShader(Utils::String2WString(FileName), nullptr, "PS", "vs_5_1");
		
		GenerateInputLayout();
	}

	void FDX12Shader::CreateBinder()
	{
	}

	void FDX12Shader::Use()
	{
	}

	void FDX12Shader::GenerateInputLayout()
	{
		for (const auto& Element : m_ShaderDesc.VertexBufferLayout)
		{
			m_InputLayoutDesc.push_back({
				Element.m_Name.c_str(), 
				0, 
				ShaderDataTypeToDXGIFormat(Element.Type), 
				0, 
				Element.Offset, 
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 
				0}
			);
		}
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

	void FDX12Shader::CreatePSO()
	{
	}
}
