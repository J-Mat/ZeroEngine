#include "ShaderBinder.h"

namespace Zero
{
    void FConstantsMapper::InsertConstant(const FBufferElement& Element, int CBIndex)
    {
        const std::string& Name = Element.m_Name;
        if (m_Mapper.find(Name) != m_Mapper.end())
        {
            CORE_LOG_ERROR("Duplicated Shader Constant Name!");
            return;
        }
        m_Mapper.insert({ Name, FShaderConstantItem(Name, Element.Type, CBIndex, Element.Offset) });
    }
    bool FConstantsMapper::FetchConstant(std::string Name, FShaderConstantItem& Buffer)
    {
        if (m_Mapper.find(Name) == m_Mapper.end())
        {
            CORE_LOG_ERROR("Shader Constant \'{0}\'Not Found!", Name);
            return false;
        }
        Buffer = m_Mapper[Name];
        return true;
    }
    void FResourcesMapper::InsertResource(const FShaderResourceItem& Element)
    {
        // If the name already exists
        std::string Name = Element.Name;
        if (m_Mapper.find(Name) != m_Mapper.end())
            CORE_LOG_ERROR("Duplicated Shader Constant Name!");
        // else insert the item
        m_Mapper.insert({ Name, Element });
    }
    bool FResourcesMapper::FetchResource(std::string Name, FShaderResourceItem& Item)
    {
        if (m_Mapper.find(Name) == m_Mapper.end())
        {
            CORE_LOG_ERROR("Shader Resource \'{0}\'Not Found!", Name);
            return false;
        }
        Item = m_Mapper[Name];
    }

    bool FResourcesMapper::SetTextureID(std::string name, std::string ID, EShaderResourceType type, unsigned int mip)
    {
        return false;
    }

    IShaderBinder::IShaderBinder(FShaderBinderDesc& Desc)
        : m_Desc(Desc)
    {
    }
    void IShaderBinder::InitMappers()
    {
        int CBIndex = 0;
        int ParaIndex = 0;
        m_ShaderConstantDescs.reserve(m_Desc.GetConstantBufferCount()); 
        for (FConstantBufferLayout ConstantLayout : m_Desc.m_ConstantBufferLayouts)
        {
            m_ShaderConstantDescs[CBIndex] = CreateRef<FShaderConstantsDesc>();
            m_ShaderConstantDescs[CBIndex]->Size = ConstantLayout.GetStride();
            for (auto BufferElement : ConstantLayout)
            {
                m_ConstantsMapper.InsertConstant(BufferElement, CBIndex);
                m_ShaderConstantDescs[CBIndex]->Mapper.InsertConstant(BufferElement, CBIndex);
            }
            CBIndex++;
            ParaIndex++;
        }

        int ResIndex = 0;
        m_ShaderResourceDescs.reserve(m_Desc.GetTextureBufferCount());
        for (FShaderResourceLayout ShaderResourceLayout : m_Desc.m_TextureBufferLayouts)
        {
            int InnerIndex = 0;
            m_ShaderResourceDescs[ResIndex] = CreateRef<FShaderResourcesDesc>();
            for (auto BufferElement : ShaderResourceLayout)
            {
                m_ShaderResourceDescs[ResIndex]->Mapper.InsertResource({BufferElement.Name, BufferElement.Type, ParaIndex++, InnerIndex, "PROCEDURE=White" });
                InnerIndex++;
            }
        }
    }
}
