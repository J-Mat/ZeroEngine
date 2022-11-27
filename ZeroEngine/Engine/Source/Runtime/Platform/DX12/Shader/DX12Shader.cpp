#include "DX12Shader.h"
#include "Utils.h"
#include "Platform/DX12/DX12Device.h"
#include "DX12ShaderBinder.h"
#include "../DX12RootSignature.h"
#include "../DX12PipelineStateObject.h"
#include "../DX12CommandList.h"

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

	FDX12Shader::FDX12Shader(std::string FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
	: IShader(BinderDesc, Desc)
	{
		m_VSBytecode = CompileShader(Utils::String2WString(FileName), nullptr, "VS", "vs_5_1");
		m_PSBytecode = CompileShader(Utils::String2WString(FileName), nullptr, "PS", "ps_5_1");
		
		GenerateInputLayout();
		CreateBinder();
	}

	void FDX12Shader::CreateBinder()
	{
		m_ShaderBinder = CreateRef<FDX12ShaderBinder>(m_ShaderBinderDesc);
	}

	void FDX12Shader::Use()
	{
		m_ShaderBinder->Bind();
	}

	void FDX12Shader::GenerateInputLayout()
	{
		m_InputLayoutDesc.clear();
		for (const auto& Element : m_ShaderDesc.VertexBufferLayout)
		{
			m_InputLayoutDesc.push_back({
				Element.Name.c_str(), 
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

	/*
	void FDX12Shader::CreatePSO()
	{
		FDX12ShaderBinder* DX12ShaderBinder = static_cast<FDX12ShaderBinder*>(m_ShaderBinder.get());
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		PsoDesc.InputLayout = { m_InputLayoutDesc.data(), (UINT)m_InputLayoutDesc.size() };
		FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(DX12ShaderBinder->GetRootSignature());
		PsoDesc.pRootSignature = D3DRootSignature->GetD3D12RootSignature().Get();
		PsoDesc.VS = { reinterpret_cast<BYTE*>(m_VSBytecode->GetBufferPointer()), m_VSBytecode->GetBufferSize() };
		PsoDesc.PS = { reinterpret_cast<BYTE*>(m_PSBytecode->GetBufferPointer()), m_PSBytecode->GetBufferSize() };
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.SampleMask = UINT_MAX;	//0xffffffff, No Sampling Mask
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.NumRenderTargets = m_ShaderDesc.NumRenderTarget;
		for (int i = 0; i < m_ShaderDesc.NumRenderTarget; i++)
			PsoDesc.RTVFormats[i] = FDX12Texture2D::GetFormatByDesc(m_ShaderDesc.Formats[i]);
		PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		PsoDesc.SampleDesc.Count = 1;	// No 4XMSAA
		PsoDesc.SampleDesc.Quality = 0;	////No 4XMSAA

		m_PipelineStateObject = CreateRef<FDX12PipelineStateObject>(PsoDesc);
	}
	*/
}
