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
            for (auto [RGTextureID, State] : Pass->m_TextureStateMap)
            { 
                m_TextureStateMap[RGTextureID] |= State;
            }
            m_BufferReads.insert(Pass->m_BufferCreates.begin(), Pass->m_BufferCreates.end());
            m_BufferDestroys.insert(Pass->m_BufferDestroys.begin(), Pass->m_BufferDestroys.end());
            for (auto [RGTextureID, State] : Pass->m_BufferStateMap)
            { 
                m_BufferStateMap[RGTextureID] |= State;
            }
        }
    }

    void FRenderGraph::FDependencyLevel::Execute()
    {
		for (auto Pass : m_Passes)
		{
			if (Pass->IsCulled())
			{
				continue;
			}
            //FRenderGraphContext RenderGraphContexts(FDependencyLevel::m_RenderGrpah, *Pass);
		}
    }

    uint32_t FRenderGraph::FDependencyLevel::GetSize()
    {
        return 0;
    }

    uint32_t FRenderGraph::FDependencyLevel::GetNonCulledSize() const
    {
        return 0;
    }

    void FRenderGraph::Build()
    {
        BuildAdjacencyLists();
        TopologicalSort();
        BuildDependencyLevels();
        CullPasses();
    }

    void FRenderGraph::SetTextureUsage(FRGTextureID RGTextureID)
    {
        for (auto& [RGTextureID, Descs] : m_SRVTexDescMap)
        { 
            Ref<FTexture2D> Texture = GetTexture(RGTextureID);
            Texture->MakeSRVs(Descs);
        }

        for (auto& [RGTextureID, Descs] : m_UAVTexDescMap)
        { 
            Ref<FTexture2D> Texture = GetTexture(RGTextureID);
            Texture->MakeUAVs(Descs);
        }

        for (auto& [RGTextureID, Descs] : m_RTVTexDescMap)
        { 
            Ref<FTexture2D> Texture = GetTexture(RGTextureID);
            Texture->MakeRTVs(Descs);
        }
    }

    void FRenderGraph::Execute()
    {
        m_ResourcePool.Tick();
        for (auto& DependencyLevel : m_DependencyLevels)
        { 
            DependencyLevel.Setup();
        }

        for (size_t i = 0; i < m_DependencyLevels.size(); ++i)
        {
            auto& DependencyLevel = m_DependencyLevels[i];
            for (auto RGTextureID : DependencyLevel.m_TextureCreates)
            {
                Ref<FRGTexture> RGTexture = GetRGTexture(RGTextureID);
                RGTexture->Resource = m_ResourcePool.AllocateTexture(RGTexture->Desc);
                SetTextureUsage(RGTextureID);
            }

            for (auto RGBufferID : DependencyLevel.m_BufferCreates)
            { 
                Ref<FRGBuffer> RGBuffer = GetRGBuffer(RGBufferID);
                RGBuffer->Resource = m_ResourcePool.AllocateBuffer(RGBuffer->Desc);
            }
        }
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

    void FRenderGraph::AddBufferBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        auto RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= Flags;
    }

    void FRenderGraph::AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
    {
        FRGTextureID Handle = m_TextureNameIDMap[Name];
        CORE_ASSERT(IsValidTextureHandle(Handle), "Resource has not been declared!");
        auto RGTexture = GetRGTexture(Handle);
        RGTexture->Desc.ResourceBindFlags |= Flags;
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
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            size_t Level = Distance[i];
            m_DependencyLevels[Level].AddPass(m_Passes[i]);
        }
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
        m_Textures.emplace_back(CreateRef<FRGTexture>(uint32_t(m_Textures.size()), TextureDesc, Name));
        auto RGTextureID = FRGTextureID(uint32_t(m_Textures.size() - 1));
        m_TextureNameIDMap[Name] = RGTextureID;
        return RGTextureID;
    }

    FRGBufferID FRenderGraph::DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc)
    {
        CORE_ASSERT(m_BufferNameIDMap.find(Name) == m_BufferNameIDMap.end(), "Buffer with that name has already been declared");
        FBufferDesc BufferDesc{};
        FillBufferDesc(Desc, BufferDesc);
        m_Buffers.emplace_back(CreateRef<FRGBuffer>(uint32_t(m_Buffers.size()), BufferDesc, Name));
        auto RGBufferID = FRGBufferID(uint32_t(m_Buffers.size() - 1));
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
        return RGTextureID.IsValid() && RGTextureID.ID < uint32_t(m_Textures.size());
    }

    bool FRenderGraph::IsValidBufferHandle(FRGBufferID RGBufferID) const
    {
        return RGBufferID.IsValid() && RGBufferID.ID < uint32_t(m_Buffers.size());
    }


    void FRenderGraph::ImportTexture(FRGResourceName Name, Ref<FTexture2D> Texture)
    {
        m_Textures.emplace_back(CreateRef<FRGTexture>(uint32_t(m_Textures.size()), Texture, Name));
        m_TextureNameIDMap[Name] = FRGTextureID(uint32_t(m_Textures.size() - 1));
    }

    void FRenderGraph::ImportBuffer(FRGResourceName Name, Ref<FBuffer> Buffer)
    {
        m_Buffers.emplace_back(CreateRef<FRGBuffer>(uint32_t(m_Buffers.size()), Buffer, Name));
        m_BufferNameIDMap[Name] = FRGBufferID(uint32_t(m_Buffers.size() - 1));
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

    FRGRenderTargetID FRenderGraph::RenderTarget(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTextureID Handle = m_TextureNameIDMap[Name];
        CORE_ASSERT(IsValidTextureHandle(Handle), "Resource has not been declared!");
        Ref<FRGTexture> RGTexture = GetRGTexture(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::RenderTarget;
        if (RGTexture->Desc.InitialState == EResourceState::Common)
        { 
            RGTexture->Desc.InitialState = EResourceState::RenderTarget;
        }
        
        std::vector <FTextureSubresourceDesc>& ViewDescs = m_RTVTexDescMap[Handle]; // m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGRenderTargetID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGRenderTargetID(ViewID, Handle);
    }

    FRGDepthStencilID FRenderGraph::DepthStencil(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTextureID Handle = m_TextureNameIDMap[Name];
        CORE_ASSERT(IsValidTextureHandle(Handle), "Resource has not been declared!");
        Ref<FRGTexture> RGTexture = GetRGTexture(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::DepthStencil;
        if (RGTexture->Desc.InitialState == EResourceState::Common)
        { 
            RGTexture->Desc.InitialState = EResourceState::DepthRead;
        }
        
        std::vector<FTextureSubresourceDesc>& ViewDescs = m_DSVTexDescMap[Handle]; //m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGDepthStencilID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGDepthStencilID(ViewID, Handle);
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

        std::vector<FTextureSubresourceDesc>& ViewDescs = m_SRVTexDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc) 
            { 
                return FRGTextureReadOnlyID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
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
        std::vector<FTextureSubresourceDesc>& ViewDescs = m_UAVTexDescMap[Handle]; // m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc) 
            { 
                return FRGTextureReadWriteID(i, Handle);
            }
        }

        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTextureReadWriteID(ViewID, Handle);
    }

    FRGBufferReadOnlyID FRenderGraph::ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        Ref<FRGBuffer> RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= EResourceBindFlag::ShaderResource;

        std::vector<FBufferSubresourceDesc>& ViewDescs = m_SRVBufferDescMap[Handle];  // m_BufferViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            auto const& [_Desc, _Type] = ViewDescs[i];
            if (ViewDescs[i] == Desc)
            { 
                return FRGBufferReadOnlyID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGBufferReadOnlyID(ViewID, Handle);
    }

    FRGBufferReadWriteID FRenderGraph::WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        Ref<FRGBuffer> RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= EResourceBindFlag::UnorderedAccess;
        std::vector<FBufferSubresourceDesc>& ViewDescs = m_UAVBufferDescMap[Handle]; //m_BufferViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGBufferReadWriteID(i, Handle);
            }
        }

        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGBufferReadWriteID(ViewID, Handle);
    }
}
