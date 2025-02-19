#include "Shader.h"
namespace Zero
{
	FShader::FShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
		:
		m_ShaderBinderDesc(BinderDesc)
		, m_ShaderDesc(Desc)
	{
		m_ShaderBinderDesc.MapCBBufferIndex();
	}

	FShader::FShader(const FShaderDesc& Desc)
	: m_ShaderDesc(Desc)
	{
	}

	void FShader::GenerateShaderDesc()
	{
		m_ShaderBinderDesc.m_ShaderSamplers =
		{
			EShaderSampler::PointWarp,
			EShaderSampler::PointClamp,
			EShaderSampler::LinearWarp,
			EShaderSampler::LinearClamp,
			EShaderSampler::AnisotropicWarp,
			EShaderSampler::AnisotropicClamp,
			EShaderSampler::Shadow
		};

		std::cout << "------------------------------------------\n";
		std::cout << m_ShaderDesc.FileName << "\n";
		for (auto Iter : m_CBVParams)
		{
			std::cout << std::format("Buffer : {0}, {1}, {2}\n", Iter.second.ResourceName, Iter.second.BindPoint, Iter.second.RegisterSpace);
			FConstantBufferLayout ConstantBufferLayout;
			ConstantBufferLayout.m_BufferName = Iter.second.ResourceName;
			for (const FCBVariableItem& CBVariableItem : Iter.second.VariableList)
			{
				FBufferElement Element = { CBVariableItem.ShaderDataType, CBVariableItem.VariableName };
				ConstantBufferLayout.m_Elements.push_back(Element);
				std::cout << std::format("Element : {0}, {1}\n", int(Element.Type), Element.Name);
			}
			ConstantBufferLayout.m_BindPoint = Iter.first.first;
			ConstantBufferLayout.m_RegisterSpace = Iter.second.RegisterSpace;
			ConstantBufferLayout.CalculateOffsetsAndStride();
			m_ShaderBinderDesc.m_ConstantBufferLayouts.push_back(ConstantBufferLayout);
		}
		
		for (auto Iter : m_SRVParams)
		{
			std::cout << std::format("SRV Name : {0}\n", Iter.second.ResourceName);
			std::cout << std::format("BindPoint : {0}\n", Iter.second.BindPoint);
			std::cout << std::format("RegisterSpace : {0}\n", Iter.second.RegisterSpace);
			FSRVElement SRVElement
			{
				.Type = Iter.second.ShaderResourceType,
				.ResourceName = Iter.second.ResourceName,
				.TextureNum = Iter.second.BindCount,
				.BindPoint = Iter.second.BindPoint,
				.RegisterSpace = Iter.second.RegisterSpace
			};
			m_ShaderBinderDesc.m_SRVResourceLayout.m_Elements.push_back(SRVElement);
		}

		for (auto Iter : m_UAVParams)
		{
			std::cout << std::format("UAV Name : {0}\n", Iter.second.ResourceName);
			std::cout << std::format("BindPoint : {0}\n", Iter.second.BindPoint);
			std::cout << std::format("RegisterSpace : {0}\n", Iter.second.RegisterSpace);
			FUAVElement UAVElement
			{
				.Type = Iter.second.ShaderResourceType,
				.ResourceName = Iter.second.ResourceName,
				.TextureNum = Iter.second.BindCount,
				.BindPoint = Iter.second.BindPoint,
				.RegisterSpace = Iter.second.RegisterSpace
			};
			m_ShaderBinderDesc.m_UAVResourceLayout.m_Elements.push_back(UAVElement);
		}
		std::cout << "------------------------------------------\n";
		
	}

	void FShaderMacro::SetDefine(const std::string& Name, const std::string& Definition)
	{
		DefinesMap.insert_or_assign(Name, Definition);
	}


	bool FShaderMacro::operator==(const FShaderMacro& Other) const
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
