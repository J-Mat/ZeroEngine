#include "Shader.h"
namespace Zero
{
	FShader::FShader(const std::string ShaderID, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc)
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
		std::cout << "------------------------------------------\n";
		for (auto Iter : m_CBVParams)
		{
			std::cout << std::format("Name : {0}\n", Iter.second.ResourceName);
			std::cout << std::format("BindPoint : {0}\n", Iter.second.BindPoint);
			FConstantBufferLayout ConstantBufferLayout;
			ConstantBufferLayout.m_BufferName = Iter.second.ResourceName;
			for (const FCBVariableItem& CBVariableItem : Iter.second.VariableList)
			{
				FBufferElement Element = { CBVariableItem.ShaderDataType, CBVariableItem.VariableName };
				ConstantBufferLayout.m_Elements.push_back(Element);
				std::cout << std::format("Element : {0}, {1}\n", int(Element.Type), Element.Name);
			}
			ConstantBufferLayout.m_BindPoint = Iter.first;
			ConstantBufferLayout.CalculateOffsetsAndStride();
			m_ShaderBinderDesc.m_ConstantBufferLayouts.push_back(ConstantBufferLayout);
		}
		
		for (auto Iter : m_SRVParams)
		{
			FTextureTableElement TextureTableElement;
			TextureTableElement.ResourceName = Iter.second.ResourceName;
			TextureTableElement.Type = Iter.second.ShaderResourceType;
			TextureTableElement.BindPoint = Iter.second.BindPoint;
			m_ShaderBinderDesc.m_TextureBufferLayout.m_Elements.push_back(TextureTableElement);
		}
		std::cout << "------------------------------------------\n";
		
	}

	void FShaderDefines::SetDefine(const std::string& Name, const std::string& Definition)
	{
		DefinesMap.insert_or_assign(Name, Definition);
	}

	bool FShaderDefines::operator==(const FShaderDefines& Other) const
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
