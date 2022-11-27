#include "ShaderBinder.h"

namespace Zero
{
    void FConstantsMapper::InsertConstant(const FBufferElement& Element)
    {
        const std::string& Name = Element.Name;
        if (m_Mapper.find(Name) != m_Mapper.end())
        {
            CORE_LOG_ERROR("Duplicated Shader Constant Name!");
            return;
        }
        m_Mapper.insert({ Name, FShaderConstantItem(Name, Element.Type, Element.Offset) });
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
        return true;
    }

    bool FResourcesMapper::SetTextureID(std::string Name, EShaderResourceType Type, unsigned int Mip)
    {
        if (m_Mapper.find(Name) == m_Mapper.end())
        {
            CORE_LOG_ERROR("Shader Resource \'{0}\'Not Found!", Name);
            return false;
        }
        m_Mapper[Name].Type = Type;
        m_Mapper[Name].SelectedMip = Mip;
        return true;
    }


    IShaderBinder::IShaderBinder(FShaderBinderDesc& Desc)
        : m_Desc(Desc)
    {
    }

    void IShaderBinder::InitMappers()
    {
        uint32_t CBIndex = 0;
        uint32_t ParaIndex = 0;
        m_ShaderConstantDescs.resize(m_Desc.GetConstantBufferCount()); 
        for (FConstantBufferLayout& ConstantLayout : m_Desc.m_ConstantBufferLayouts)
        {
            m_ShaderConstantDescs[CBIndex] = CreateRef<FShaderConstantsDesc>();
            m_ShaderConstantDescs[CBIndex]->Size = ConstantLayout.GetStride();
            for (auto BufferElement : ConstantLayout)
            {
                m_ConstantsMapper.InsertConstant(BufferElement);
                m_ShaderConstantDescs[CBIndex]->Mapper.InsertConstant(BufferElement);
            }
            CBIndex++;
            ParaIndex++;
        }

        m_ShaderResourceDesc = CreateRef<FShaderResourcesDesc>();
        uint32_t ResIndex = 0;
        for (FTextureTableElement& Element : m_Desc.m_TextureBufferLayout)
        {
            FShaderResourceItem ShaderResourceItem = { Element.Name, Element.Type, ParaIndex, 0, Element.TextureNum};
            m_ResourcesMapper.InsertResource(ShaderResourceItem);
            m_ShaderResourceDesc->Mapper.InsertResource(ShaderResourceItem);
            ResIndex++;
            ParaIndex++;
        }
    }
}
