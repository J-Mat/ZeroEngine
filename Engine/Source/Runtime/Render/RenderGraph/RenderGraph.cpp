#include "RenderGraph.h"
#include "../RenderPass/RenderPass.h"
#include "Render/RendererAPI.h"
#include "EnumUtil.h"


namespace Zero
{
	FRenderGraph::FRenderGraph(FRGResourcePool& Pool)
		:m_ResourcePool(Pool)
	{

	}

	FRenderGraph::~FRenderGraph()
	{
        /*
        for (auto& [RGTextureID, _] : m_SRVTex2DDescMap)
        { 
            FTexture2D* Texture = GetTexture2D(RGTextureID);
            Texture->ReleaseSRVs();
        }

        for (auto& [RGTextureID, _] : m_UAVTex2DDescMap)
        { 
            FTexture2D* Texture = GetTexture2D(RGTextureID);
            Texture->ReleaseUAVs();
        }

        for (auto& [RGTextureID, _] : m_RTVTex2DDescMap)
        { 
            FTexture2D* Texture = GetTexture2D(RGTextureID);
            Texture->ReleaseRTVs();
        }

        for (auto& [RGTextureID, _] : m_DSV2DTexDescMap)
        { 
            FTexture2D* Texture = GetTexture2D(RGTextureID);
            Texture->ReleaseDSVs();
        }

        for (auto& [RGTextureID, _] : m_SRVTexCubeDescMap)
        { 
            FTextureCube* Texture = GetTextureCube(RGTextureID);
            Texture->ReleaseSRVs();
        }

        for (auto& [RGTextureID, _] : m_UAVTexCubeDescMap)
        { 
            FTextureCube* Texture = GetTextureCube(RGTextureID);
            Texture->ReleaseUAVs();
        }

        for (auto& [RGTextureID, _] : m_RTVTexCubeDescMap)
        { 
            FTextureCube* Texture = GetTextureCube(RGTextureID);
            Texture->ReleaseRTVs();
        }

        for (auto& [RGTextureID, _] : m_DSV2DTexDescMap)
        { 
            FTexture2D* Texture = GetTexture2D(RGTextureID);
            Texture->ReleaseDSVs();
        }
        */
	}

	void FRenderGraph::FDependencyLevel::AddPass(Ref<FRGPassBase> Pass)
    {
        m_Passes.push_back(Pass);
        m_Texture2DReads.insert(Pass->m_Texture2DReads.begin(), Pass->m_Texture2DReads.end());
        m_Texture2DWrites.insert(Pass->m_Texture2DWrites.begin(), Pass->m_Texture2DWrites.end());
        m_TextureCubeReads.insert(Pass->m_TextureCubeReads.begin(), Pass->m_TextureCubeReads.end());
        m_TextureCubeWrites.insert(Pass->m_TextureCubeWrites.begin(), Pass->m_TextureCubeWrites.end());
        m_BufferReads.insert(Pass->m_BufferReads.begin(), Pass->m_BufferReads.end());
        m_BufferWrites.insert(Pass->m_BufferWrites.begin(), Pass->m_BufferWrites.end());
    }

    void FRenderGraph::FDependencyLevel::Setup()
    {
        for (auto Pass : m_Passes)
        { 
            if (Pass->IsCulled())
                continue;
            m_Texture2DCreates.insert(Pass->m_Texture2DCreates.begin(), Pass->m_Texture2DCreates.end());
            m_Texture2DDestroys.insert(Pass->m_Texture2DDestroys.begin(), Pass->m_Texture2DDestroys.end());
            m_TextureCubeCreates.insert(Pass->m_TextureCubeCreates.begin(), Pass->m_TextureCubeCreates.end());
            m_TextureCubeDestroys.insert(Pass->m_TextureCubeDestroys.begin(), Pass->m_TextureCubeDestroys.end());
            m_BufferCreates.insert(Pass->m_BufferCreates.begin(), Pass->m_BufferCreates.end());
            m_BufferDestroys.insert(Pass->m_BufferDestroys.begin(), Pass->m_BufferDestroys.end());
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
            FRenderGraphContext RenderGraphContexts(m_RenderGrpah, *Pass);
               FCommandListHandle CommandListHandle = FGraphic::GetDevice()->GetSingleThreadCommadList(ERenderPassType::Graphics);
            if (Pass->GetPassType() == ERenderPassType::Graphics && !Pass->SkipAutoRenderPassSetup())
            {
                FRenderPassDesc RenderPassDesc;
                if (Pass->AllowUAVWrites())
                { 
                    //RenderPassDesc.
                }
                for (auto const& RenderTargetInfo : Pass->m_RenderTergetInfo)
                {
                    if (Pass->GetRenderPassRTType() == ERenderPassRTType::Texuture2D)
                    {
                        FRGTexture2DID RGTextureID = RenderTargetInfo.RGRenderTarget2DID->GetResourceID();
                        FTexture2D* Texture = m_RenderGrpah.GetTexture2D(RGTextureID);

                        ERGLoadAccessOp LoadAccess = ERGLoadAccessOp::NoAccess;
                        ERGStoreAccessOp StoreAccess = ERGStoreAccessOp::NoAccess;
                        SplitAccessOp(RenderTargetInfo.RenderTargetAccess, LoadAccess, StoreAccess);

                        const FTextureDesc& TexDesc = Texture->GetDesc();
                        FRtvAttachmentDesc RtvDesc =
                        {
                            .RTTexture2D = Texture,
                            .BeginningAccess = LoadAccess,
                            .EndingAccess = StoreAccess,
                            .ClearValue = TexDesc.ClearValue,
                        };
                        RenderPassDesc.RtvAttachments.push_back(RtvDesc);
                    }
                    else
                    {
                        CORE_ASSERT(Pass->GetRenderPassRTType() == ERenderPassRTType::TexutureCube, "RGCubeRenderTargetCubeID must have value");
                        FRGTextureCubeID RGTextureID = RenderTargetInfo.RGRenderTargetCubeID->GetResourceID();
                        FTextureCube* Texture = m_RenderGrpah.GetTextureCube(RGTextureID);

                        ERGLoadAccessOp LoadAccess = ERGLoadAccessOp::NoAccess;
                        ERGStoreAccessOp StoreAccess = ERGStoreAccessOp::NoAccess;
                        SplitAccessOp(RenderTargetInfo.RenderTargetAccess, LoadAccess, StoreAccess);

                        const FTextureDesc& TexDesc = Texture->GetDesc();
                        FRtvAttachmentDesc RtvDesc =
                        {
                            .RTTextureCube = Texture,
                            .BeginningAccess = LoadAccess,
                            .EndingAccess = StoreAccess,
                            .ClearValue = TexDesc.ClearValue,
                        };
                        RenderPassDesc.RtvAttachments.push_back(RtvDesc);

                    }
                }

                if (Pass->m_DepthStencil.has_value())
                {
                    auto DepthStencilInfo = Pass->m_DepthStencil.value();
                    if (Pass->GetRenderPassRTType() == ERenderPassRTType::Texuture2D)
                    {
                        FRGTexture2DID RGTextureID = DepthStencilInfo.RGTex2DDepthStencilID->GetResourceID();
                        FTexture2D* Texture = m_RenderGrpah.GetTexture2D(RGTextureID);

					    ERGLoadAccessOp LoadAccess = ERGLoadAccessOp::NoAccess;
					    ERGStoreAccessOp StoreAccess = ERGStoreAccessOp::NoAccess;
                        SplitAccessOp(DepthStencilInfo.DepthAccess, LoadAccess, StoreAccess);

                        const FTextureDesc& TexDesc = Texture->GetDesc();

                        FDsvAttachmentDesc DsvDesc = 
                        {
                            .DSTexture2D = Texture,
                            .DepthBeginningAccess = LoadAccess,
                            .DepthEndingAccess = StoreAccess,
                            .ClearValue = TexDesc.ClearValue,
                        };
                        RenderPassDesc.DsvAttachment = std::move(DsvDesc);
                    }
                    else
                    {
                        FRGTextureCubeID RGTextureID = DepthStencilInfo.RGTexCubeDepthStencilID->GetResourceID();
                        FTextureCube* Texture = m_RenderGrpah.GetTextureCube(RGTextureID);

                        ERGLoadAccessOp LoadAccess = ERGLoadAccessOp::NoAccess;
                        ERGStoreAccessOp StoreAccess = ERGStoreAccessOp::NoAccess;
                        SplitAccessOp(DepthStencilInfo.DepthAccess, LoadAccess, StoreAccess);

                        const FTextureDesc& TexDesc = Texture->GetDesc();

                        FDsvAttachmentDesc DsvDesc =
                        {
                            .DSTextureCube = Texture,
                            .DepthBeginningAccess = LoadAccess,
                            .DepthEndingAccess = StoreAccess,
                            .ClearValue = TexDesc.ClearValue,
                        };
                        RenderPassDesc.DsvAttachment = std::move(DsvDesc);
                    }
                }

                //CORE_ASSERT(Pass->m_VieportWidth != 0 && Pass->m_VieportHeight != 0, "Viewport Width/Height is 0! The call to builder.SetViewport is probably missing...");
                RenderPassDesc.Width = Pass->m_VieportWidth;
                RenderPassDesc.Height = Pass->m_VieportHeight;
                RenderPassDesc.Depth = Pass->m_VieportDepth;
                RenderPassDesc.RenderPassRTType = Pass->m_RenderPassRTType;
                FRenderPass RenderPass(m_RenderGrpah, RenderPassDesc);
                RenderPass.Begin(CommandListHandle);
                Pass->Execute(RenderGraphContexts, CommandListHandle);
                RenderPass.End(CommandListHandle);
            }
            else
            {
                Pass->Execute(RenderGraphContexts, CommandListHandle);
            }
		}
    }


	uint32_t FRenderGraph::FDependencyLevel::GetSize()
    {
        return uint32_t(m_Passes.size());
    }

    uint32_t FRenderGraph::FDependencyLevel::GetNonCulledSize() const
    {
        return 0;
        /*
        return std::count_if(
            std::begin(m_Passes),
            std::end(m_Passes),
            [](const FRGPassBase* Pass)
            {
                return !Pass->IsCulled();
            }
        );
        */
    }


	void FRenderGraph::Build()
    {
        BuildAdjacencyLists();
        TopologicalSort();
        BuildDependencyLevels();
        CullPasses();
        CalculateResourcesLifetime();
    }

    void FRenderGraph::CreateTexture2DViews(FRGTexture2DID RGTextureID)
    {
        FTexture2D* Texture = GetTexture2D(RGTextureID);
        if (auto Iter = m_SRVTex2DDescMap.find(RGTextureID); Iter != m_SRVTex2DDescMap.end())
        {
            Texture->MakeSRVs(Iter->second);
        }

        if (auto Iter = m_UAVTex2DDescMap.find(RGTextureID); Iter != m_UAVTex2DDescMap.end())
        { 
            Texture->MakeUAVs(Iter->second);
        }

        if (auto Iter = m_RTVTex2DDescMap.find(RGTextureID); Iter != m_RTVTex2DDescMap.end())
        {
            Texture->MakeRTVs(Iter->second);
        }

        if (auto Iter = m_DSV2DTexDescMap.find(RGTextureID); Iter !=  m_DSV2DTexDescMap.end())
        { 
            Texture->MakeDSVs(Iter->second);
        }
    }


	void FRenderGraph::CreateTextureCubeViews(FRGTextureCubeID RGTextureCubeID)
	{
        FTextureCube* Texture = GetTextureCube(RGTextureCubeID);
        if (auto Iter = m_SRVTexCubeDescMap.find(RGTextureCubeID); Iter != m_SRVTexCubeDescMap.end())
        {
            Texture->MakeSRVs(Iter->second);
        }

        if (auto Iter = m_UAVTexCubeDescMap.find(RGTextureCubeID); Iter != m_UAVTexCubeDescMap.end())
        { 
            Texture->MakeUAVs(Iter->second);
        }

        if (auto Iter = m_RTVTexCubeDescMap.find(RGTextureCubeID); Iter != m_RTVTexCubeDescMap.end())
        {
            Texture->MakeRTVs(Iter->second);
        }

        if (auto Iter = m_DSVTexCubeDescMap.find(RGTextureCubeID); Iter !=  m_DSVTexCubeDescMap.end())
        { 
            Texture->MakeDSVs(Iter->second);
        }
	}

	void FRenderGraph::CreateBufferViews(FRGBufferID RGBufferID)
	{
        FBuffer* Buffer = GetBuffer(RGBufferID);
        if (auto Iter = m_SRVBufferDescMap.find(RGBufferID); Iter != m_SRVBufferDescMap.end())
        {
            const auto& SubDesc = Iter->second;
            Buffer->CreateSRV(SubDesc);
        }

        if (auto Iter = m_UAVBufferDescMap.find(RGBufferID);Iter != m_UAVBufferDescMap.end())
        {
            const auto& SubDesc = Iter->second;
            Buffer->CreateUAV(SubDesc);
        }
	}

	void FRenderGraph::DestroyTextureResource(FDependencyLevel& DependencyLevel)
	{
        for (FRGTexture2DID RGTextureID : DependencyLevel.m_Texture2DDestroys)
        {
            FRGTexture2D* RGTexture = GetRGTexture2D(RGTextureID);
            FTexture2D* Texture = RGTexture->Resource;
            if (!RGTexture->bImported)
            {
                m_ResourcePool.ReleaseTexture2D(Texture);
            }
        }
        for (FRGTextureCubeID RGTextureID : DependencyLevel.m_TextureCubeDestroys)
        {
            FRGTextureCube* RGTexture = GetRGTextureCube(RGTextureID);
            FTextureCube* Texture = RGTexture->Resource;
            if (!RGTexture->bImported)
            {
                m_ResourcePool.ReleaseTextureCube(Texture);
            }
        }
        for (FRGBufferID RGBufferID : DependencyLevel.m_BufferDestroys)
        {
            FRGBuffer* RGTexture = GetRGBuffer(RGBufferID);
            FBuffer* Buffer = RGTexture->Resource;
            if (!RGTexture->bImported)
            {
                m_ResourcePool.ReleaseBuffer(Buffer);
            }
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
            for (auto RGTextureID : DependencyLevel.m_Texture2DCreates)
            {
                FRGTexture2D* RGTexture = GetRGTexture2D(RGTextureID);
                RGTexture->Resource = m_ResourcePool.AllocateTexture2D(RGTexture->Desc, RGTexture->Name);
                CreateTexture2DViews(RGTextureID);
            }

            for (auto RGTextureID : DependencyLevel.m_TextureCubeCreates)
            {
                FRGTextureCube* RGTexture = GetRGTextureCube(RGTextureID);
                RGTexture->Resource = m_ResourcePool.AllocateTextureCube(RGTexture->Desc, RGTexture->Name);
                CreateTextureCubeViews(RGTextureID);
            }

            for (auto RGBufferID : DependencyLevel.m_BufferCreates)
            { 
                FRGBuffer* RGBuffer = GetRGBuffer(RGBufferID);
                RGBuffer->Resource = m_ResourcePool.AllocateBuffer(RGBuffer->Desc);
                CreateBufferViews(RGBufferID);
            }
            DependencyLevel.Execute();
           
            DestroyTextureResource(DependencyLevel);
        }

    }

    FRGTexture2DID FRenderGraph::GetTexture2DID(FRGResourceName Name)
    {
        CORE_ASSERT(!IsTexture2DDeclared(Name), std::format("{0} is not Declared!", Name.Name));
        return m_Texture2DNameIDMap[Name];
    }

	FRGTextureCubeID FRenderGraph::GetTextureCubeID(FRGResourceName Name)
	{
        CORE_ASSERT(!IsTextureCubeDeclared(Name), std::format("{0} is not Declared!", Name.Name));
        return m_TextureCubeNameIDMap[Name];
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

    void FRenderGraph::AddTexture2DBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
    {
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        auto RGTexture = GetRGTexture2D(Handle);
        RGTexture->Desc.ResourceBindFlags |= Flags;
    }

	void FRenderGraph::AddTextureCubeBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
	{
        FRGTextureCubeID Handle = m_TextureCubeNameIDMap[Name];
        CORE_ASSERT(IsValidTextureCubeHandle(Handle), "Resource has not been declared!");
        auto RGTexture = GetRGTextureCube(Handle);
        RGTexture->Desc.ResourceBindFlags |= Flags;
	}

	void FRenderGraph::BuildAdjacencyLists()
    {
        m_AdjacencyList.resize(m_Passes.size());
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            auto& CurPass = m_Passes[i];
            std::vector<uint64_t>& PassAdjacencyList = m_AdjacencyList[i];
            for (size_t j = i + 1; j < m_Passes.size(); ++j)
            {
                auto& OtherPass = m_Passes[j];
                bool bDepends = false;
                for (auto OtherNodeRead : OtherPass->m_Texture2DReads)
                {
                    if (CurPass->m_Texture2DWrites.find(OtherNodeRead) != CurPass->m_Texture2DWrites.end())
                    {
                        PassAdjacencyList.push_back(j);
                        bDepends = true;
                        break;
                    }
                }

                if (bDepends)
                    continue;
                
                for (auto OtherNodeRead : OtherPass->m_TextureCubeReads)
                {
                    if (CurPass->m_TextureCubeWrites.find(OtherNodeRead) != CurPass->m_TextureCubeWrites.end())
                    {
                        PassAdjacencyList.push_back(j);
                        bDepends = true;
                        break;
                    }
                }

                if (bDepends)
                    continue;

                for (auto OtherNodeRead : OtherPass->m_BufferReads)
                {
                    if (CurPass->m_BufferWrites.find(OtherNodeRead) != CurPass->m_BufferWrites.end())
                    {
                        PassAdjacencyList.push_back(j);
                        break;
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

	void FRenderGraph::CalculateResourcesLifetime()
	{
        for (size_t i = 0;i < m_TopologicallySortedPasses.size(); ++i)
        {
            auto& Pass = m_Passes[m_TopologicallySortedPasses[i]];
            if (Pass->IsCulled())
            {
                continue;
            }
            {
                for (auto ID : Pass->m_Texture2DWrites)
                {
                    FRGTexture2D* RGTexture = GetRGTexture2D(ID);
                    RGTexture->LastPassUsedBy = Pass.get();
                }
                for (auto ID : Pass->m_Texture2DReads)
                {
                    FRGTexture2D* RGTexture = GetRGTexture2D(ID);
                    RGTexture->LastPassUsedBy = Pass.get();
                }
            }

            {
                for (auto ID : Pass->m_TextureCubeWrites)
                {
                    FRGTextureCube* RGTexture = GetRGTextureCube(ID);
                    RGTexture->LastPassUsedBy = Pass.get();
                }
                for (auto ID : Pass->m_TextureCubeReads)
                {
                    FRGTextureCube* RGTexture = GetRGTextureCube(ID);
                    RGTexture->LastPassUsedBy = Pass.get();
                }
            }
            {
                for (auto ID : Pass->m_BufferWrites)
                {
                    FRGBuffer* RGBuffer = GetRGBuffer(ID);
                    RGBuffer->LastPassUsedBy = Pass.get();
                }
                for (auto ID : Pass->m_BufferReads)
                {
                    FRGBuffer* RGBuffer = GetRGBuffer(ID);
                    RGBuffer->LastPassUsedBy = Pass.get();
                }
            }
        }
        for (uint32_t i = 0; i < m_Texture2Ds.size(); ++i)
        {
            if (m_Texture2Ds[i]->LastPassUsedBy != nullptr)
            {
                m_Texture2Ds[i]->LastPassUsedBy->m_Texture2DDestroys.insert(FRGTexture2DID(i));
                if (m_Texture2Ds[i]->bImported)
                {
                    CreateTexture2DViews(FRGTexture2DID(i));
                }
            }
        }
        for (uint32_t i = 0; i < m_TextureCubes.size(); ++i)
        {
            if (m_TextureCubes[i]->LastPassUsedBy != nullptr)
            {
                m_TextureCubes[i]->LastPassUsedBy->m_TextureCubeDestroys.insert(FRGTextureCubeID(i));
                if (m_TextureCubes[i]->bImported)
                {
                    CreateTextureCubeViews(FRGTextureCubeID(i));
                }
            }
        }
        for (uint32_t i = 0; i < m_Buffers.size(); ++i)
        {
            if (m_Buffers[i]->LastPassUsedBy != nullptr)
            {
                m_Buffers[i]->LastPassUsedBy->m_BufferDestroys.insert(FRGBufferID(i));
                if (m_Buffers[i]->bImported)
                {
                    CreateBufferViews(FRGBufferID(i));
                }
            }
        }
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

    FRGTexture2DID FRenderGraph::DeclareTexture2D(FRGResourceName Name, const FRGTextureDesc& Desc)
    {
        CORE_ASSERT(m_Texture2DNameIDMap.find(Name) == m_Texture2DNameIDMap.end(), "Texture2D with that name has already been declared");
        FTextureDesc TextureDesc{};
        FillTextureDesc(Desc, TextureDesc);
        m_Texture2Ds.emplace_back(new FRGTexture2D(uint32_t(m_Texture2Ds.size()), TextureDesc, Name));
        auto RGTextureID = FRGTexture2DID(uint32_t(m_Texture2Ds.size() - 1));
        m_Texture2DNameIDMap[Name] = RGTextureID;
        return RGTextureID;
    }

	FRGTextureCubeID FRenderGraph::DeclareTextureCube(FRGResourceName Name, const FRGTextureDesc& Desc)
	{
        CORE_ASSERT(m_TextureCubeNameIDMap.find(Name) == m_TextureCubeNameIDMap.end(), "TextureCube with that name has already been declared");
        FTextureDesc TextureDesc{};
        FillTextureDesc(Desc, TextureDesc);
        m_TextureCubes.emplace_back(new FRGTextureCube(uint32_t(m_TextureCubes.size()), TextureDesc, Name));
        auto RGTextureID = FRGTextureCubeID(uint32_t(m_TextureCubes.size() - 1));
        m_TextureCubeNameIDMap[Name] = RGTextureID;
        return RGTextureID;
	}

	FRGBufferID FRenderGraph::DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc)
    {
        CORE_ASSERT(m_BufferNameIDMap.find(Name) == m_BufferNameIDMap.end(), "Buffer with that name has already been declared");
        FBufferDesc BufferDesc{};
        FillBufferDesc(Desc, BufferDesc);
        m_Buffers.emplace_back(new FRGBuffer(uint32_t(m_Buffers.size()), BufferDesc, Name));
        auto RGBufferID = FRGBufferID(uint32_t(m_Buffers.size() - 1));
        m_BufferNameIDMap[Name] = RGBufferID;
        return RGBufferID;
    }

    bool FRenderGraph::IsTexture2DDeclared(FRGResourceName ResourceName)
    {
        return  m_Texture2DNameIDMap.find(ResourceName) == m_Texture2DNameIDMap.end();
    }

	bool FRenderGraph::IsTextureCubeDeclared(FRGResourceName ResourceName)
	{
        return  m_TextureCubeNameIDMap.find(ResourceName) == m_TextureCubeNameIDMap.end();
	}

	bool FRenderGraph::IsBufferDeclared(FRGResourceName ResourceName)
    {
        return m_BufferNameIDMap.find(ResourceName) == m_BufferNameIDMap.end();
    }

    bool FRenderGraph::IsValidTexture2DHandle(FRGTexture2DID RGTextureID) const
    {
        return RGTextureID.IsValid() && RGTextureID.ID < uint32_t(m_Texture2Ds.size());
    }

	bool FRenderGraph::IsValidTextureCubeHandle(FRGTextureCubeID RGTextureID) const
	{
        return RGTextureID.IsValid() && RGTextureID.ID < uint32_t(m_TextureCubes.size());
	}

	bool FRenderGraph::IsValidBufferHandle(FRGBufferID RGBufferID) const
    {
        return RGBufferID.IsValid() && RGBufferID.ID < uint32_t(m_Buffers.size());
    }


    void FRenderGraph::ImportTexture2D(FRGResourceName Name, FTexture2D* Texture)
    {
        m_Texture2Ds.emplace_back(new FRGTexture2D(uint32_t(m_Texture2Ds.size()), Texture, Name));
        m_Texture2DNameIDMap[Name] = FRGTexture2DID(uint32_t(m_Texture2Ds.size() - 1));
    }

    void FRenderGraph::ImportBuffer(FRGResourceName Name, FBuffer* Buffer)
    {
        m_Buffers.emplace_back(new FRGBuffer(uint32_t(m_Buffers.size()), Buffer, Name));
        m_BufferNameIDMap[Name] = FRGBufferID(uint32_t(m_Buffers.size() - 1));
    }

    FRGTexture2D* FRenderGraph::GetRGTexture2D(FRGTexture2DID RGTextureID) const
    {
        return m_Texture2Ds[RGTextureID.ID].get();
    }

	FRGTextureCube* FRenderGraph::GetRGTextureCube(FRGTextureCubeID RGTextureID) const
	{
        return m_TextureCubes[RGTextureID.ID].get();
	}

	FTexture2D* FRenderGraph::GetTexture2D(FRGTexture2DID RGTextureID)
    {
        return GetRGTexture2D(RGTextureID)->Resource;
    }

	FTextureCube* FRenderGraph::GetTextureCube(FRGTextureCubeID RGTextureID)
	{
        return GetRGTextureCube(RGTextureID)->Resource;
	}

	FRGBuffer* FRenderGraph::GetRGBuffer(FRGBufferID RGBufferID) const
    {
        return m_Buffers[RGBufferID.ID].get();
    }

    FBuffer* FRenderGraph::GetBuffer(FRGBufferID RGBufferID)
    {
        return GetRGBuffer(RGBufferID)->Resource;
    }

	FRGTexture2DCopySrcID FRenderGraph::ReadCopySrcTexture2D(FRGResourceName Name)
	{
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        return FRGTexture2DCopySrcID(Handle);
	}

	FRGTexture2DCopyDstID FRenderGraph::WriteCopyDstTexture2D(FRGResourceName Name)
	{
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        return FRGTexture2DCopyDstID(Handle);
	}

    FRGTex2DRenderTargetID FRenderGraph::RenderTarget2D(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        FRGTexture2D* RGTexture = GetRGTexture2D(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::RenderTarget;
        
        std::vector <FTextureSubresourceDesc>& ViewDescs = m_RTVTex2DDescMap[Handle]; // m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGTex2DRenderTargetID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTex2DRenderTargetID(ViewID, Handle);
    }

    FRGTex2DDepthStencilID FRenderGraph::DepthStencil2D(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        FRGTexture2D* RGTexture = GetRGTexture2D(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::DepthStencil;
        
        std::vector<FTextureSubresourceDesc>& ViewDescs = m_DSV2DTexDescMap[Handle]; //m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGTex2DDepthStencilID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTex2DDepthStencilID(ViewID, Handle);
    }

    FRGTex2DReadOnlyID FRenderGraph::ReadTexture2D(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        FRGTexture2D* RGTexture = GetRGTexture2D(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::ShaderResource;

        std::vector<FTextureSubresourceDesc>& ViewDescs = m_SRVTex2DDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc) 
            { 
                return FRGTex2DReadOnlyID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTex2DReadOnlyID(ViewID, Handle);
    }

    FRGTex2DReadWriteID FRenderGraph::WriteTexture2D(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
    {
        FRGTexture2DID Handle = m_Texture2DNameIDMap[Name];
        CORE_ASSERT(IsValidTexture2DHandle(Handle), "Resource has not been declared!");
        FRGTexture2D* RGTexture = GetRGTexture2D(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::ShaderResource;
        std::vector<FTextureSubresourceDesc>& ViewDescs = m_UAVTex2DDescMap[Handle]; // m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc) 
            { 
                return FRGTex2DReadWriteID(i, Handle);
            }
        }

        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTex2DReadWriteID(ViewID, Handle);
    }

	FRGTexCubeRenderTargetID FRenderGraph::RenderTargetCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
        FRGTextureCubeID Handle = m_TextureCubeNameIDMap[Name];
        CORE_ASSERT(IsValidTextureCubeHandle(Handle), "Resource has not been declared!");
        FRGTextureCube* RGTexture = GetRGTextureCube(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::RenderTarget;
        
        std::vector <FTextureSubresourceDesc>& ViewDescs = m_RTVTexCubeDescMap[Handle]; // m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGTexCubeRenderTargetID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTexCubeRenderTargetID(ViewID, Handle);
	}

	FRGTexCubeDepthStencilID FRenderGraph::DepthStencilCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
        FRGTextureCubeID Handle = m_TextureCubeNameIDMap[Name];
        CORE_ASSERT(IsValidTextureCubeHandle(Handle), "Resource has not been declared!");
        FRGTextureCube* RGTexture = GetRGTextureCube(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::DepthStencil;
        
        std::vector<FTextureSubresourceDesc>& ViewDescs = m_DSVTexCubeDescMap[Handle]; //m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc)
            { 
                return FRGTexCubeDepthStencilID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTexCubeDepthStencilID(ViewID, Handle);
	}

	FRGTexCubeReadOnlyID FRenderGraph::ReadTextureCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
        FRGTextureCubeID Handle = m_TextureCubeNameIDMap[Name];
        CORE_ASSERT(IsValidTextureCubeHandle(Handle), "Resource has not been declared!");
        FRGTextureCube* RGTexture = GetRGTextureCube(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::ShaderResource;

        std::vector<FTextureSubresourceDesc>& ViewDescs = m_SRVTexCubeDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc) 
            { 
                return FRGTexCubeReadOnlyID(i, Handle);
            }
        }
        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTexCubeReadOnlyID(ViewID, Handle);
	}

	FRGTexCubeReadWriteID FRenderGraph::WriteTextureCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
        FRGTextureCubeID Handle = m_TextureCubeNameIDMap[Name];
        CORE_ASSERT(IsValidTextureCubeHandle(Handle), "Resource has not been declared!");
        FRGTextureCube* RGTexture = GetRGTextureCube(Handle);
        RGTexture->Desc.ResourceBindFlags |= EResourceBindFlag::ShaderResource;
        std::vector<FTextureSubresourceDesc>& ViewDescs = m_UAVTexCubeDescMap[Handle]; // m_TextureViewDescMap[Handle];
        for (uint32_t i = 0; i < ViewDescs.size(); ++i)
        { 
            if (ViewDescs[i] == Desc) 
            { 
                return FRGTexCubeReadWriteID(i, Handle);
            }
        }

        ViewDescs.emplace_back(Desc);
        uint32_t ViewID = uint32_t(ViewDescs.size() - 1);
        return FRGTexCubeReadWriteID(ViewID, Handle);
	}

	FRGBufferReadOnlyID FRenderGraph::ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        FRGBuffer* RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= EResourceBindFlag::ShaderResource;

        m_SRVBufferDescMap[Handle] = Desc;
        return FRGBufferReadOnlyID(0, Handle);
    }

    FRGBufferReadWriteID FRenderGraph::WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
    {
        FRGBufferID Handle = m_BufferNameIDMap[Name];
        CORE_ASSERT(IsValidBufferHandle(Handle), "Resource has not been declared!");
        FRGBuffer* RGBuffer = GetRGBuffer(Handle);
        RGBuffer->Desc.ResourceBindFlag |= EResourceBindFlag::UnorderedAccess;
        m_UAVBufferDescMap[Handle] = Desc;
        return FRGBufferReadWriteID(0, Handle);
    }
}
