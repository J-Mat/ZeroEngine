#include "RenderGraph.h"

namespace Zero
{
    void FRenderGraph::FDependencyLevel::AddPass(Ref<FRGPassBase> Pass)
    {
        m_Passes.push_back(Pass);
        m_TextureReads.insert(Pass->m_TextureReads.begin(), Pass->m_TextureReads.end());
        m_TextureWrites.insert(Pass->m_TextureWrites.begin(), Pass->m_TextureWrites.end());
        m_BufferReads.insert(Pass->m_BufferReads.begin(), Pass->m_BufferReads.end());
        m_BufferWrites.insert(Pass->m_BufferWrites.begin(), Pass->m_BufferWrites.end());
    }

    void FRenderGraph::FDependencyLevel::Setup()
    {
        for (auto Pass : m_Passes)
        { 
            if (Pass->IsCulled())
                continue;
            m_TextureReads.insert(Pass->m_TextureCreates.begin(), Pass->m_TextureCreates.end());
            m_TextureDestroys.insert(Pass->m_TextureDestroys.begin(), Pass->m_TextureDestroys.end());
        }
    }

    void FRenderGraph::FDependencyLevel::Execute()
    {
    }

    FRGTextureID FRenderGraph::GetTextureID(FRGResourceName Name)
    {
        CORE_ASSERT(!IsTextureDeclared(Name), std::format("{0} is not Declared!", Name.Name));
        return m_TextureNameIDMap[Name];
    }

    FRGBufferID FRenderGraph::GetBufferID(FRGResourceName Name)
    {
        CORE_ASSERT(!IsBufferDeclared(Name), std::format("{0} is not Declared!", Name.Name));
        return m_BufferNameIDMap[Name];
    }

    void FRenderGraph::BuildAdjacencyLists()
    {
        m_AdjacencyList.resize(m_Passes.size());
        for (size_t i = 0; m_Passes.size(); ++i)
        {
            auto& CurPass = m_Passes[i];
            std::vector<uint64_t>& PassAdjacencyList = m_AdjacencyList[i];
            for (size_t j = i + 1; m_Passes.size(); ++j)
            {
                auto& OtherPass = m_Passes[j];
                bool bDepends = false;
                for (auto OtherNodeRead : OtherPass->m_TextureReads)
                {
                    if (CurPass->m_TextureWrites.find(OtherNodeRead) != CurPass->m_TextureWrites.end())
                    {
                        PassAdjacencyList.push_back(j);
                        bDepends = true;
                    }
                }

                if (bDepends)
                    continue;
                
                for (auto OtherNodeRead : OtherPass->m_BufferReads)
                {
                    if (CurPass->m_BufferWrites.find(OtherNodeRead) != CurPass->m_BufferWrites.end())
                    {
                        PassAdjacencyList.push_back(j);
                        bDepends = true;
                    }
                }
            }
        }
    }

    void FRenderGraph::TopologicalSort()
    {
        std::stack<size_t> Stack{};
        std::vector<bool> Visited(m_Passes.size(), false);
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            if (!Visited[i])
            { 
                DepthFirstSearch(i, Visited, Stack);
            }
        }
        
        while (!Stack.empty())
        {
            size_t i = Stack.top();
            m_TopologicallySortedPasses.push_back(i);
            Stack.pop();
        }
    }

    void FRenderGraph::BuildDependencyLevels()
    {
        std::vector<size_t> Distance(m_TopologicallySortedPasses.size(), 0);
        for (auto i : m_TopologicallySortedPasses)
        {
            for (auto j : m_AdjacencyList[i])
            { 
               if (Distance[j] < Distance[i] + 1)
               { 
                   Distance[j] = Distance[i] + 1;
               }
            }
        }
        
        size_t MaxLevel = *std::max_element(std::begin(Distance), std::end(Distance)) + 1;
        m_DependencyLevels.resize(MaxLevel, FDependencyLevel(*this)); 
    }

    void FRenderGraph::CullPasses()
    {
    }

    void FRenderGraph::DepthFirstSearch(size_t i, std::vector<bool>& Visited, std::stack<size_t>& Stack)
    {
        Visited[i] = true;
        for (size_t j : m_AdjacencyList[i])
        {
            if (!Visited[j])
                DepthFirstSearch(j, Visited, Stack);
        }
        Stack.push(i);
    }

    FRGTextureID FRenderGraph::DeclareTexture(FRGResourceName Name, const FRGTextureDesc& Desc)
    {
        CORE_ASSERT(m_TextureNameIDMap.find(Name) == m_TextureNameIDMap.end(), "Texture with that name has already been declared");
        FTextureDesc TextureDesc{};
        FillTextureDesc(Desc, TextureDesc);
        m_Textures.emplace_back(CreateRef<FRGTexture>(m_Textures.size(), TextureDesc, Name));
        auto RGTextureID = FRGTextureID(m_Textures.size() - 1);
        m_TextureNameIDMap[Name] = RGTextureID;
        return RGTextureID;
    }

    FRGBufferID FRenderGraph::DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc)
    {
        CORE_ASSERT(m_BufferNameIDMap.find(Name) == m_BufferNameIDMap.end(), "Buffer with that name has already been declared");
        FBufferDesc BufferDesc{};
        FillBufferDesc(Desc, BufferDesc);
        m_Buffers.emplace_back(CreateRef<FRGBuffer>(m_Buffers.size(), BufferDesc, Name));
        auto RGBufferID = FRGBufferID(m_Buffers.size() - 1);
        m_BufferNameIDMap[Name] = RGBufferID;
        return RGBufferID;
    }

    bool FRenderGraph::IsTextureDeclared(FRGResourceName ResourceName)
    {
        return  m_TextureNameIDMap.find(ResourceName) == m_TextureNameIDMap.end();
    }

    bool FRenderGraph::IsBufferDeclared(FRGResourceName ResourceName)
    {
        return m_BufferNameIDMap.find(ResourceName) == m_BufferNameIDMap.end();
    }

    bool FRenderGraph::IsValidTextureHandle(FRGTextureID RGTextureID) const
    {
        return RGTextureID.IsValid() && RGTextureID.ID < m_Textures.size();
    }

    bool FRenderGraph::IsValidBufferHandle(FRGBufferID RGBufferID) const
    {
        return RGBufferID.IsValid() && RGBufferID.ID < m_Buffers.size();
    }

    void FRenderGraph::ImportTexture(FRGResourceName Name, Ref<FTexture2D> Texture)
    {
        m_Textures.emplace_back(CreateRef<FRGTexture>(m_Textures.size(), Texture, Name));
        m_TextureNameIDMap[Name] = FRGTextureID(m_Textures.size() - 1);
    }

    void FRenderGraph::ImportBuffer(FRGResourceName Name, Ref<FBuffer> Buffer)
    {
        m_Buffers.emplace_back(CreateRef<FRGBuffer>(m_Buffers.size(), Buffer, Name));
        m_BufferNameIDMap[Name] = FRGBufferID(m_Buffers.size() - 1);
    }

    Ref<FRGTexture> FRenderGraph::GetRGTexture(FRGTextureID RGTextureID) const
    {
        return m_Textures[RGTextureID.ID];
    }

    Ref<FTexture2D> FRenderGraph::GetTexture(FRGTextureID RGTextureID)
    {
        return GetRGTexture(RGTextureID)->Resource;
    }

    Ref<FRGBuffer> FRenderGraph::GetRGBuffer(FRGBufferID RGBufferID) const
    {
        return m_Buffers[RGBufferID.ID];
    }

    Ref<FBuffer> FRenderGraph::GetBuffer(FRGBufferID RGBufferID)
    {
        return GetRGBuffer(RGBufferID)->Resource;
    }

    FRGTextureReadOnlyID FRenderGraph::ReadTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTextureID Handle = m_TextureNameIDMap[Name];
        CORE_ASSERT(IsValidTextureHandle(Handle), "Resource has not been declared!");
        Ref<FRGTexture> RGTexture = GetRGTexture(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::ShaderResource;
        if (RGTexture->Desc.InitialState == EResourceState::Common)
        { 
            RGTexture->Desc.InitialState = EResourceState::PixelShaderResource | EResourceState::NonPixelShaderResource;
        }

        std::vector<std::pair<FTextureSubresourceDesc, ERGDescriptorType>>& ViewDescs = m_TextureViewDescMap[Handle];
        for (size_t i = 0; i < ViewDescs.size(); ++i)
        { 
            auto const& [_Desc, _Type] = ViewDescs[i];
            if (Desc == _Desc && _Type == ERGDescriptorType::ReadOnly)
            { 
                return FRGTextureReadOnlyID(i, Handle);
            }
        }
        size_t ViewID = ViewDescs.size();
        ViewDescs.emplace_back(Desc, ERGDescriptorType::ReadOnly);
        return FRGTextureReadOnlyID(ViewID, Handle);
    }

    FRGTextureReadWriteID FRenderGraph::WriteTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTextureID Handle = m_TextureNameIDMap[Name];
        CORE_ASSERT(IsValidTextureHandle(Handle), "Resource has not been declared!");
        Ref<FRGTexture> RGTexture = GetRGTexture(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::ShaderResource;
        if (RGTexture->Desc.InitialState == EResourceState::Common)
        { 
            RGTexture->Desc.InitialState = EResourceState::UnorderedAccess;
        }
        std::vector<std::pair<FTextureSubresourceDesc, ERGDescriptorType>>& ViewDescs = m_TextureViewDescMap[Handle];
        for (size_t i = 0; i < ViewDescs.size(); ++i)
        { 
            auto const& [_Desc, _Type] = ViewDescs[i];
            if (Desc == _Desc && _Type == ERGDescriptorType::ReadWrite)
            { 
                return FRGTextureReadWriteID(i, Handle);
            }
        }

        size_t ViewID = ViewDescs.size();
        ViewDescs.emplace_back(Desc, ERGDescriptorType::ReadWrite);
        return FRGTextureReadWriteID(ViewID, Handle);
    }

    FRGBufferReadOnlyID FRenderGraph::ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        Ref<FRGBuffer> RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= EResourceBindFlag::ShaderResource;

        std::vector<std::pair<FBufferSubresourceDesc, ERGDescriptorType>>& ViewDescs = m_BufferViewDescMap[Handle];
        for (size_t i = 0; i < ViewDescs.size(); ++i)
        { 
            auto const& [_Desc, _Type] = ViewDescs[i];
            if (Desc == _Desc && _Type == ERGDescriptorType::ReadOnly)
            { 
                return FRGBufferReadOnlyID(i, Handle);
            }
        }
        size_t ViewID = ViewDescs.size();
        ViewDescs.emplace_back(Desc, ERGDescriptorType::ReadOnly);
        return FRGBufferReadOnlyID(ViewID, Handle);
    }

    FRGBufferReadWriteID FRenderGraph::WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        Ref<FRGBuffer> RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= EResourceBindFlag::UnorderedAccess;
        std::vector<std::pair<FBufferSubresourceDesc, ERGDescriptorType>>& ViewDescs = m_BufferViewDescMap[Handle];
        for (size_t i = 0; i < ViewDescs.size(); ++i)
        { 
            auto const& [_Desc, _Type] = ViewDescs[i];
            if (Desc == _Desc && _Type == ERGDescriptorType::ReadWrite)
            { 
                return FRGBufferReadWriteID(i, Handle);
            }
        }

        size_t ViewID = ViewDescs.size();
        ViewDescs.emplace_back(Desc, ERGDescriptorType::ReadWrite);
        return FRGBufferReadWriteID(ViewID, Handle);
    }

    FResourceCpuHandle FRenderGraph::GetRenderTarget(FRGRenderTargetID ID) const
    {
        FRGTextureID TexturID = ID.GetResourceID();
        auto const& Views = m_TextureViewMap[TexturID];
        return Views[ID.GetViewID()].first;
    }

    FResourceCpuHandle FRenderGraph::GetDepthStencil(FRGRenderTargetID ID) const
    {
        FRGTextureID TexturID = ID.GetResourceID();
        auto const& Views = m_TextureViewMap[TexturID];
        return Views[ID.GetViewID()].first;
    }

    FResourceCpuHandle FRenderGraph::GetReadOnlyTexture(FRGTextureReadOnlyID ID) const
    {
        FRGTextureID TexturID = ID.GetResourceID();
        auto const& Views = m_TextureViewMap[TexturID];
        return Views[ID.GetViewID()].first;
    }

    FResourceCpuHandle FRenderGraph::GetReadWriteTexture(FRGTextureReadWriteID ID) const
    {
        FRGTextureID TexturID = ID.GetResourceID();
        auto const& Views = m_TextureViewMap[TexturID];
        return Views[ID.GetViewID()].first;
    }

    FResourceCpuHandle FRenderGraph::GetReadOnlyBuffer(FRGBufferReadOnlyID ID) const
    {
        FRGBufferID TexturID = ID.GetResourceID();
        auto const& Views = m_BufferViewMap[TexturID];
        return Views[ID.GetViewID()].first;
    }

    FResourceCpuHandle FRenderGraph::GetReadWriteBuffer(FRGBufferReadWriteID ID) const
    {
        FRGBufferID TexturID = ID.GetResourceID();
        auto const& Views = m_BufferViewMap[TexturID];
        return Views[ID.GetViewID()].first;
    }
}
