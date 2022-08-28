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
    void IShaderBinder::InitMappers(const FShaderBinderDesc& Desc)
    {
        int CBIndex = 0;
        int ParaIndex = 0;
        m_ShaderConstantDescs = new FShaderConstantsDesc[Desc.GetConstantBufferCount()];
        for (FConstantBufferLayout ConstantLayout : Desc.m_ConstantBufferLayouts)
        {
            m_ShaderConstantDescs[CBIndex].Size = ConstantLayout.GetStride();
            for (auto BufferElement : ConstantLayout)
            {
                m_ConstantsMapper.InsertConstant(BufferElement, CBIndex);
                m_ShaderConstantDescs[CBIndex].Mapper.InsertConstant(BufferElement, CBIndex);
            }
            CBIndex++;
            ParaIndex++;
        }

        int ResIndex = 0;
        m_ShaderResourceDescs = new FShaderResourcesDesc[Desc.GetTextureBufferCount()];
        for (FShaderResourceLayout ShaderResourceLayout : Desc.m_TextureBufferLayouts)
        {
            int InnerIndex = 0;
            for (auto BufferElement : ShaderResourceLayout)
            {
                m_ShaderResourceDescs[ResIndex].Mapper.InsertResource({BufferElement.Name, BufferElement.Type, ParaIndex++, InnerIndex, "PROCEDURE=White" });
                InnerIndex;
            }
        }
    }
}
