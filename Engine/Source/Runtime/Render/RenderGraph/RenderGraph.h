#pragma once
#include "Core.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphResourcePool.h"
#include "Render/ResourceCommon.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/Buffer.h"

namespace Zero
{
	class FRenderGraph
	{
		friend class FRenderGraphBuilder;
		friend class FRenderGraphContext;
		
		class FDependencyLevel
		{
			friend FRenderGraph;
		public:
			explicit FDependencyLevel(FRenderGraph& Rg) : m_RenderGrpah(Rg) {};
			void AddPass(Ref<FRGPassBase> Pass);
			void Setup();
			void Execute(FCommandListHandle ComandListHandle);
			uint32_t GetSize();
			uint32_t GetNonCulledSize() const;
			FRenderGraph& m_RenderGrpah;
		private:
			std::vector<Ref<FRGPassBase>> m_Passes;

			std::set<FRGTextureID> m_TextureCreates;
			std::set<FRGTextureID> m_TextureReads;
			std::set<FRGTextureID> m_TextureWrites;
			std::set<FRGTextureID> m_TextureDestroys;
			std::map<FRGTextureID, EResourceState> m_TextureStateMap;

			std::set<FRGBufferID> m_BufferCreates;
			std::set<FRGBufferID> m_BufferReads;
			std::set<FRGBufferID> m_BufferWrites;
			std::set<FRGBufferID> m_BufferDestroys;
			std::map<FRGBufferID, EResourceState> m_BufferStateMap;
		};
	public:
		FRenderGraph(FRGResourcePool& Pool) :
			m_ResourcePool(Pool)
		{}

		template<typename PassData, typename... FArgs> requires std::is_constructible_v<FRederGraphPass<PassData>, FArgs...>
		decltype(auto) AddPass(FArgs&&... Args)
		{
			m_Passes.emplace_back(CreateScope<FRederGraphPass<PassData>>(std::forward<FArgs>(Args)...));
			FRenderGraphBuilder Builder(*this, *m_Passes.back());
			m_Passes.back()->Setup(Builder);
			return *dynamic_cast<FRederGraphPass<PassData>*>(m_Passes.back().get());
		}

		void Build();
		void Execute();

		FRGResourcePool& GetResourcePool() { return m_ResourcePool; };

	private:
		FRGResourcePool m_ResourcePool;
		std::vector<Ref<FRGPassBase>> m_Passes;
		std::vector<FRGTexture*> m_Textures;
		std::vector<FRGBuffer*> m_Buffers;

		std::vector<std::vector<size_t>> m_AdjacencyList;
		std::vector<size_t> m_TopologicallySortedPasses;
		std::vector<FDependencyLevel> m_DependencyLevels;


		std::map<FRGResourceName, FRGTextureID> m_TextureNameIDMap;
		std::map<FRGResourceName, FRGBufferID>  m_BufferNameIDMap;
		std::map<FRGBufferReadWriteID, FRGBufferID> m_BufferUavCounterMap;

		// mutable std::map<FRGTextureID, std::vector<std::pair<FTextureSubresourceDesc, ERGDescriptorType>>> m_TextureViewDescMap;
		std::map<FRGTextureID, std::vector<FTextureSubresourceDesc>> m_SRVTexDescMap;
		std::map<FRGTextureID, std::vector<FTextureSubresourceDesc>> m_DSVTexDescMap;
		std::map<FRGTextureID, std::vector<FTextureSubresourceDesc>> m_RTVTexDescMap;
		std::map<FRGTextureID, std::vector<FTextureSubresourceDesc>> m_UAVTexDescMap;
		
		//mutable std::map<FRGBufferID, std::vector<std::pair<FBufferSubresourceDesc, ERGDescriptorType>>> m_BufferViewDescMap;

		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_SRVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_DSVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_RTVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_UAVBufferDescMap;

	private:
		FRGTextureID GetTextureID(FRGResourceName Name);
		FRGBufferID GetBufferID(FRGResourceName Name);
		void AddBufferBindFlags(FRGResourceName Name,  EResourceBindFlag Flags);
		void AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags);

		void BuildAdjacencyLists();
		void TopologicalSort();
		void BuildDependencyLevels();
		void CullPasses();
		void DepthFirstSearch(size_t i, std::vector<bool>& visited, std::stack<size_t>& Stack);
		
		FRGTextureID DeclareTexture(FRGResourceName Name, const FRGTextureDesc& Desc);
		FRGBufferID DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc);
		
		bool IsTextureDeclared(FRGResourceName ResourceName);
		bool IsBufferDeclared(FRGResourceName ResourceName);

		bool IsValidTextureHandle(FRGTextureID RGTextureID) const;
		bool IsValidBufferHandle(FRGBufferID RGBufferID) const;
		
		void CreateTextureViews(FRGTextureID RGTextureID);
		
		void ImportTexture(FRGResourceName Name, FTexture2D* Texture);
		void ImportBuffer(FRGResourceName Name, FBuffer* Buffer);
		
		inline FRGTexture* GetRGTexture(FRGTextureID RGTextureID) const;
		FTexture2D* GetTexture(FRGTextureID RGTextureID);
		inline FRGBuffer* GetRGBuffer(FRGBufferID RGBufferID) const;
		FBuffer* GetBuffer(FRGBufferID RGBufferID);
		
		FRGRenderTargetID RenderTarget(FRGResourceName name, const FTextureSubresourceDesc& desc);
		FRGDepthStencilID DepthStencil(FRGResourceName name, const FTextureSubresourceDesc& desc);
		FRGTextureReadOnlyID ReadTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc); 
		FRGTextureReadWriteID WriteTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGBufferReadOnlyID ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
	};
}