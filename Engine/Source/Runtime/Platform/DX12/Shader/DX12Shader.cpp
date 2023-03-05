#include "DX12Shader.h"
#include "DX12ShaderCompiler.h"
#include "Utils.h"
#include "Platform/DX12/DX12Device.h"
#include "DX12ShaderBinder.h"
#include "../DX12RootSignature.h"
#include "../PSO/DX12PipelineStateObject.h"
#include "../DX12CommandList.h"
#include "DX12ShaderCompiler.h"
#include "ZConfig.h"

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

	FDX12Shader::FDX12Shader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
	: FShader(BinderDesc, Desc)
	{
		CORE_ASSERT(false, "false");
		/*
		std::string FileName = ZConfig::GetAssestsFullPath(m_ShaderDesc.FileName).string();

		if (m_ShaderDesc.bCreateVS)
		{
			auto VSBlob  = FDX12ShaderCompiler::Get().CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.EntryPoint[uint32_t(EShaderStage::VS)], "vs_5_1");
			m_ShaderPass[EShaderStage::VS] = VSBlob;
		}

		if (m_ShaderDesc.bCreatePS)
		{
			auto PSBlob  = FDX12ShaderCompiler::Get().CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.EntryPoint[uint32_t(EShaderStage::PS)], "ps_5_1");
			m_ShaderPass[EShaderStage::PS] = PSBlob;
		}

		GenerateInputLayout();
		CreateBinder();
		*/
	}

	FDX12Shader::FDX12Shader(const FShaderDesc& Desc)
		:FShader(Desc)
	{
		Compile();
	}

	void FDX12Shader::Compile()
	{
		std::string FileName = ZConfig::GetAssestsFullPath(m_ShaderDesc.FileName).string();
		m_CBVParams.clear();
		m_SRVParams.clear();
		m_UAVParams.clear();
		m_SamplerParams.clear();

		/*
		if (m_ShaderDesc.bCreateVS)
		{
			auto VSBlob  = FDX12ShaderCompiler::Get().CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.EntryPoint[uint32_t(EShaderStage::VS)], "vs_5_1");
			m_ShaderPass[EShaderStage::VS] = VSBlob;
			std::cout << "ShaderStage : VS" << std::endl;
			GetShaderParameters(VSBlob, EShaderStage::VS);
		}

		if (m_ShaderDesc.bCreatePS)
		{
			auto PSBlob  = FDX12ShaderCompiler::Get().CompileShader(Utils::String2WString(FileName), nullptr, m_ShaderDesc.EntryPoint[uint32_t(EShaderStage::PS)], "ps_5_1");
			m_ShaderPass[EShaderStage::PS] = PSBlob;
			std::cout << "ShaderStage : PS" << std::endl;
			GetShaderParameters(PSBlob, EShaderStage::PS);
		}
		*/
		
		// Test
		{
				m_CBVParams.clear();
				m_SRVParams.clear();
				m_UAVParams.clear();
				m_SamplerParams.clear();
			{
				FShaderCompileOutput VSOutput
				{
					.Shader = this
				};
				FDX12ShaderCompiler::Get().CompileShader(EShaderStage::VS, m_ShaderDesc, VSOutput, m_IncludeFiles);
				ParseShader(VSOutput.ShaderReflection.Get(), EShaderStage::VS);
			}

			{
				FShaderCompileOutput PSOutput
				{
					.Shader = this
				};
				FDX12ShaderCompiler::Get().CompileShader(EShaderStage::PS, m_ShaderDesc, PSOutput, m_IncludeFiles);
				ParseShader(PSOutput.ShaderReflection.Get(), EShaderStage::PS);
			}
			m_IncludeFiles.insert(ZConfig::GetAssestsFullPath(m_ShaderDesc.FileName).string());
		}

		GenerateShaderDesc();
		m_ShaderBinderDesc.MapCBBufferIndex();
		GenerateInputLayout();
		CreateBinder();

	}

	void FDX12Shader::CreateBinder()
	{
		m_ShaderBinder.reset();
		m_ShaderBinder = CreateRef<FDX12ShaderBinder>(m_ShaderBinderDesc);
	}


	void FDX12Shader::Use(FCommandListHandle CommandListHandle)
	{
		m_ShaderBinder->Bind(CommandListHandle);
	}


	void FDX12Shader::GetShaderParameters(ComPtr<ID3DBlob> PassBlob, EShaderStage ShaderStage)
	{
		if (PassBlob == nullptr)
		{
			return;
		}
		ID3D12ShaderReflection* ReflectionPtr = nullptr;
		D3DReflect(PassBlob->GetBufferPointer(), PassBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&ReflectionPtr);
		
		D3D12_SHADER_DESC D3DShaderDesc;
		ReflectionPtr->GetDesc(&D3DShaderDesc);

		ParseShader(ReflectionPtr, ShaderStage);

		ReflectionPtr->Release();
	}

	void FDX12Shader::ParseShader(ID3D12ShaderReflection* ShaderReflectionPtr, EShaderStage ShaderStage)
	{
		D3D12_SHADER_DESC D3DShaderDesc;
		ShaderReflectionPtr->GetDesc(&D3DShaderDesc);
		for (UINT i = 0; i < D3DShaderDesc.BoundResources; i++)
		{
			D3D12_SHADER_INPUT_BIND_DESC  ResourceDesc;
			ShaderReflectionPtr->GetResourceBindingDesc(i, &ResourceDesc);

			LPCSTR ResoureceVarName = ResourceDesc.Name;
			D3D_SHADER_INPUT_TYPE ResourceType = ResourceDesc.Type;
			UINT RegisterSpace = ResourceDesc.Space;
			UINT BindPoint = ResourceDesc.BindPoint;
			UINT BindCount = ResourceDesc.BindCount;

			std::pair<uint32_t, uint32_t> Key = { ResourceDesc.BindPoint,  ResourceDesc.Space };

			if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER)
			{
				if (m_CBVParams.find(Key) != m_CBVParams.end())
				{
					continue;
				}
				FShaderCBVParameter Param;
				Param.ResourceName = ResoureceVarName;
				Param.ShaderStage = ShaderStage;
				Param.BindPoint = BindPoint;
				Param.RegisterSpace = RegisterSpace;

				ID3D12ShaderReflectionConstantBuffer* Buffer = ShaderReflectionPtr->GetConstantBufferByName(ResoureceVarName);
				D3D12_SHADER_BUFFER_DESC BufferDesc;
				Buffer->GetDesc(&BufferDesc);
				for (UINT BufferIndex = 0; BufferIndex < BufferDesc.Variables; ++BufferIndex)
				{
					ID3D12ShaderReflectionVariable* ReflectionVariable = Buffer->GetVariableByIndex(BufferIndex);
					ParseCBVariable(ReflectionVariable, Param.VariableList);
				}
				m_CBVParams.insert({Key, Param});
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED)
			{
				if (m_SRVParams.find(Key) != m_SRVParams.end())
				{
					continue;
				}
				FShaderSRVParameter Param;
				Param.ResourceName = ResoureceVarName;
				Param.ShaderStage = ShaderStage;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;

				m_SRVParams.insert({Key, Param });
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE)
			{
				if (m_SRVParams.find(Key) != m_SRVParams.end())
				{
					continue;
				}
				D3D12_SHADER_INPUT_BIND_DESC Desc;
				ShaderReflectionPtr->GetResourceBindingDesc(i, &Desc);
				FShaderSRVParameter Param;
				Param.ResourceName = ResoureceVarName;
				Param.ShaderStage = ShaderStage;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;
				Param.ShaderResourceType = FDX12Utils::GetResourceByDimension(Desc.Dimension);

				m_SRVParams.insert({Key, Param });
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED)
			{
				CORE_ASSERT(ShaderStage == EShaderStage::CS, "ShaderStage must be Compute Shader");

				FShaderUAVParameter Param;
				Param.ResourceName = ResoureceVarName;
				Param.ShaderStage = ShaderStage;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;

				m_UAVParams.push_back(Param);
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED)
			{
				CORE_ASSERT(ShaderStage == EShaderStage::CS, "ShaderStage must be Compute Shader");

				FShaderUAVParameter Param;
				Param.ResourceName = ResoureceVarName;
				Param.ShaderStage = ShaderStage;
				Param.BindPoint = BindPoint;
				Param.BindCount = BindCount;
				Param.RegisterSpace = RegisterSpace;

				m_UAVParams.push_back(Param);
			}
			else if (ResourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER)
			{
				CORE_ASSERT(ShaderStage == EShaderStage::PS, "ShaderStage must be Pixel Shader");

				FShaderSamplerParameter Param;
				Param.ResourceName = ResoureceVarName;
				Param.ShaderStage = ShaderStage;
				Param.BindPoint = BindPoint;
				Param.RegisterSpace = RegisterSpace;
				m_SamplerParams.push_back(Param);
			}
		}
	}



	void FDX12Shader::ParseCBVariable(ID3D12ShaderReflectionVariable* ReflectionVariable, std::vector<FCBVariableItem>& VariableList)
	{
		D3D12_SHADER_VARIABLE_DESC VariableDesc;
		ReflectionVariable->GetDesc(&VariableDesc);
		ID3D12ShaderReflectionType* ReflectionType = ReflectionVariable->GetType();
		D3D12_SHADER_TYPE_DESC TypeDesc;
		ReflectionType->GetDesc(&TypeDesc);

		if (TypeDesc.Elements > 0)
		{
			for (UINT ArrayIndex = 0; ArrayIndex < TypeDesc.Elements; ++ArrayIndex)
			{
				if (TypeDesc.Class == D3D_SVC_STRUCT)
				{
					for (UINT StructTypeIndex = 0; StructTypeIndex < TypeDesc.Members; ++StructTypeIndex)
					{
						auto MemberTypeName = ReflectionType->GetMemberTypeName(StructTypeIndex);
						ID3D12ShaderReflectionType* MemberType = ReflectionType->GetMemberTypeByName(MemberTypeName);
						D3D12_SHADER_TYPE_DESC MemberTypeDesc;
						MemberType->GetDesc(&MemberTypeDesc);

						FCBVariableItem VariableItem;
						VariableItem.VariableName = std::format("{0}[{1}].{2}", VariableDesc.Name, ArrayIndex, MemberTypeName);
						VariableItem.ShaderDataType = FDX12Utils::GetTypeByName(MemberTypeDesc.Name);
						VariableItem.VariableTypeName = MemberTypeDesc.Name;
						VariableList.push_back(VariableItem);
					}
				}
				else
				{
					FCBVariableItem VariableItem;
					VariableItem.VariableName = std::format("{0}[{1}]", VariableDesc.Name, ArrayIndex);
					VariableItem.ShaderDataType = FDX12Utils::GetTypeByName(TypeDesc.Name);
					VariableItem.VariableTypeName = TypeDesc.Name;
					VariableList.push_back(VariableItem);
				}
			}
		}
		else
		{
			if (TypeDesc.Class == D3D_SVC_STRUCT)
			{
				for (UINT StructTypeIndex = 0; StructTypeIndex < TypeDesc.Members; ++StructTypeIndex)
				{
					auto MemberTypeName = ReflectionType->GetMemberTypeName(StructTypeIndex);
					ID3D12ShaderReflectionType* MemberType = ReflectionType->GetMemberTypeByName(MemberTypeName);
					D3D12_SHADER_TYPE_DESC MemberTypeDesc;
					MemberType->GetDesc(&MemberTypeDesc);

					FCBVariableItem VariableItem;
					VariableItem.VariableName = std::format("{0}.{2}", VariableDesc.Name, MemberTypeName);
					VariableItem.ShaderDataType = FDX12Utils::GetTypeByName(MemberTypeDesc.Name);
					VariableItem.VariableTypeName = MemberTypeDesc.Name;
					VariableList.push_back(VariableItem);
				}
			}
			else
			{
				FCBVariableItem VariableItem;
				VariableItem.VariableName = VariableDesc.Name;
				VariableItem.ShaderDataType = FDX12Utils::GetTypeByName(TypeDesc.Name);
				VariableItem.VariableTypeName = TypeDesc.Name;
				VariableList.push_back(VariableItem);
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
}
