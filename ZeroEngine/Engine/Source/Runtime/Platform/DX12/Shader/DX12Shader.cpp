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

	FDX12Shader::FDX12Shader(const std::string& FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
	: FShader(FileName, BinderDesc, Desc)
	{
		m_ShaderPass[EShaderType::ST_VERTEX] = CompileShader(Utils::String2WString(FileName), nullptr, "VS", "vs_5_1");
		m_ShaderPass[EShaderType::ST_PIXEL] = CompileShader(Utils::String2WString(FileName), nullptr, "PS", "ps_5_1");
		
		GenerateInputLayout();
		CreateBinder();

		if (m_ShaderDesc.bCreateVS)
		{
			auto VSBlob  = CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.VSEntryPoint, "vs_5_1");
			m_ShaderPass[EShaderType::ST_VERTEX] = VSBlob;
		}

		if (m_ShaderDesc.bCreatePS)
		{
			auto PSBlob  = CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.PSEntryPoint, "ps_5_1");
			m_ShaderPass[EShaderType::ST_PIXEL] = PSBlob;
		}

		if (m_ShaderDesc.bCreateCS)
		{
			auto CSBlob  = CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.CSEntryPoint, "cs_5_1");
			m_ShaderPass[EShaderType::ST_COMPUTE] = CSBlob;
			//GetShaderParameters(CSBlob, EShaderType::ST_COMPUTE);
		}
	}

	FDX12Shader::FDX12Shader(const std::string& FileName, const FShaderDesc& Desc)
		:FShader(Desc)
	{
		if (m_ShaderDesc.bCreateVS)
		{
			auto VSBlob  = CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.VSEntryPoint, "vs_5_1");
			m_ShaderPass[EShaderType::ST_VERTEX] = VSBlob;
			std::cout << "ShaderType : VS" << std::endl;
			GetShaderParameters(VSBlob, EShaderType::ST_VERTEX);
		}

		if (m_ShaderDesc.bCreatePS)
		{
			auto PSBlob  = CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.PSEntryPoint, "ps_5_1");
			m_ShaderPass[EShaderType::ST_PIXEL] = PSBlob;
			std::cout << "ShaderType : PS" << std::endl;
			GetShaderParameters(PSBlob, EShaderType::ST_PIXEL);
		}

		if (m_ShaderDesc.bCreateCS)
		{
			auto CSBlob  = CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.CSEntryPoint, "cs_5_1");
			m_ShaderPass[EShaderType::ST_COMPUTE] = CSBlob;
			std::cout << "ShaderType : CS" << std::endl;
			GetShaderParameters(CSBlob, EShaderType::ST_COMPUTE);
		}
		std::cout << "------------------------------------------\n";
	
		std::cout << std::format("File : {0}\n", FileName);
		for (auto Iter : m_CBVParams)
		{ 
			std::cout << std::format("BindPoint : {0}\n", Iter.first);
			const FShaderCBVParameter& Parameter = Iter.second;
			std::cout << std::format("Name : {0}\n", Parameter.Name);
			std::cout << std::format("BindPoint : {0}\n", Parameter.BindPoint);
			std::cout << std::format("RegisterSpace : {0}\n", Parameter.RegisterSpace);
			
			for (const FCBVariableItem& Item : Parameter.VariableList)
			{ 
				std::cout << std::format("\tName : {0}\n", Item.VariableName);
				std::cout << std::format("\tType : {0}\n", int(Item.ShaderDataType));
			}
		}
		/*
		std::cout << "-------------\n";
		for (auto Iter : m_SRVParams)
		{
			std::cout << std::format("BindPoint : {0}\n", Iter.first);
			const FShaderSRVParameter& Parameter = Iter.second;
			std::cout << std::format("Name : {0}\n", Parameter.Name);
			std::cout << std::format("BindPoint : {0}\n", Parameter.BindPoint);
			std::cout << std::format("BindCount : {0}\n", Parameter.BindCount);
			std::cout << std::format("RegisterSpace : {0}\n", Parameter.RegisterSpace);
			std::cout << std::format("Dimension : {0}\n", int(Parameter.ShaderResourceType));
		}
		*/

		std::cout << "------------------------------------------\n";
	}

	void FDX12Shader::CreateBinder()
	{
		m_ShaderBinder = CreateRef<FDX12ShaderBinder>(m_ShaderBinderDesc);
	}

	void FDX12Shader::Use()
	{
		m_ShaderBinder->Bind();
	}

	void FDX12Shader::GetShaderParameters(ComPtr<ID3DBlob> PassBlob, EShaderType ShaderType)
	{
		ID3D12ShaderReflection* Reflection = nullptr;
		D3DReflect(PassBlob->GetBufferPointer(), PassBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&Reflection);
		
		D3D12_SHADER_DESC D3DShaderDesc;
		Reflection->GetDesc(&D3DShaderDesc);


		for (UINT i = 0; i < D3DShaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC  ResourceDesc;
			Reflection->GetResourceBindingDesc(i, &ResourceDesc);




			LPCSTR ResoureceVarName = ResourceDesc.Name;
			D3D_SHADER_INPUT_TYPE ResourceType = ResourceDesc.Type;
			UINT RegisterSpace = ResourceDesc.Space;
			UINT BindPoint = ResourceDesc.BindPoint;
			UINT BindCount = ResourceDesc.BindCount;


			if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
			{
				if (m_CBVParams.find(BindPoint) != m_CBVParams.end())
				{
					continue;
				}
				FShaderCBVParameter Param;
				Param.Name = ResoureceVarName;
				Param.ShaderType = ShaderType;
				Param.BindPoint = BindPoint;
				Param.RegisterSpace = RegisterSpace;

				ID3D12ShaderReflectionConstantBuffer* Buffer = Reflection->GetConstantBufferByName(ResoureceVarName);
				D3D12_SHADER_BUFFER_DESC BufferDesc;
				Buffer->GetDesc(&BufferDesc);
				for (UINT BufferIndex = 0; BufferIndex < BufferDesc.Variables; ++BufferIndex)
				{ 
					ID3D12ShaderReflectionVariable* ReflectionVariable = Buffer->GetVariableByIndex(BufferIndex);
					D3D12_SHADER_VARIABLE_DESC VariableDesc;
					ReflectionVariable->GetDesc(&VariableDesc);
					ID3D12ShaderReflectionType*  Type = ReflectionVariable->GetType();
					D3D12_SHADER_TYPE_DESC TypeDesc;
					Type->GetDesc(&TypeDesc);

					if (TypeDesc.Class == D3D_SVC_STRUCT)
					{
						for (UINT StructTypeIndex = 0; StructTypeIndex < TypeDesc.Members; ++StructTypeIndex)
						{
							auto MemberTypeName = Type->GetMemberTypeName(StructTypeIndex);
							ID3D12ShaderReflectionType* MemberType = Type->GetMemberTypeByName(MemberTypeName);
							D3D12_SHADER_TYPE_DESC MemberTypeDesc;
							MemberType->GetDesc(&MemberTypeDesc);
							std::cout << "struct: " << MemberTypeName << std::endl;
							std::cout << "type: " << (int)FDX12RHItConverter::GetTypeByName(MemberTypeDesc.Name) << std::endl;
						}
						
						FCBVariableItem CBVariableItem;
						CBVariableItem.VariableName = VariableDesc.Name;
						CBVariableItem.ShaderDataType = FDX12RHItConverter::GetTypeByName(TypeDesc.Name);

						Param.VariableList.push_back(CBVariableItem);
					}
					else
					{
						FCBVariableItem CBVariableItem;
						CBVariableItem.VariableName = VariableDesc.Name;
						CBVariableItem.ShaderDataType = FDX12RHItConverter::GetTypeByName(TypeDesc.Name);
						Param.VariableList.push_back(CBVariableItem);
					}
				}
				
				m_CBVParams.insert({BindPoint, Param });
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED)
			{
				if (m_SRVParams.find(BindPoint) != m_SRVParams.end())
				{
					continue;
				}
				FShaderSRVParameter Param;
				Param.Name = ResoureceVarName;
				Param.ShaderType = ShaderType;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;

				m_SRVParams.insert({BindPoint, Param });
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
			{
				if (m_SRVParams.find(BindPoint) != m_SRVParams.end())
				{
					continue;
				}
				D3D12_SHADER_INPUT_BIND_DESC Desc;
				Reflection->GetResourceBindingDesc(i, &Desc);
				FShaderSRVParameter Param;
				Param.Name = ResoureceVarName;
				Param.ShaderType = ShaderType;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;
				Param.ShaderResourceType = FDX12RHItConverter::GetResourceByDimension(Desc.Dimension);

				m_SRVParams.insert({BindPoint, Param });
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED)
			{
				CORE_ASSERT(ShaderType == EShaderType::ST_COMPUTE, "ShaderType must be Compute Shader");

				FShaderUAVParameter Param;
				Param.Name = ResoureceVarName;
				Param.ShaderType = ShaderType;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;

				m_UAVParams.push_back(Param);
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED)
			{
				CORE_ASSERT(ShaderType == EShaderType::ST_COMPUTE, "ShaderType must be Compute Shader");

				FShaderUAVParameter Param;
				Param.Name = ResoureceVarName;
				Param.ShaderType = ShaderType;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;

				m_UAVParams.push_back(Param);
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
			{
				CORE_ASSERT(ShaderType == EShaderType::ST_PIXEL, "ShaderType must be Pixel Shader");

				FShaderSamplerParameter Param;
				Param.Name = ResoureceVarName;
				Param.ShaderType = ShaderType;
				Param.BindPoint = BindPoint;
				Param.RegisterSpace = RegisterSpace;
				m_SamplerParams.push_back(Param);
			}
		}
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
}
