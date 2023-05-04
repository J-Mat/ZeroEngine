#pragma once
#include "Core.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphResourcePool.h"
#include "Render/ResourceCommon.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/Buffer.h"

namespace Zero
{
	class FResourceBarrierBatch;
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
			void Execute();
			uint32_t GetSize();
			uint32_t GetNonCulledSize() const;
			FRenderGraph& m_RenderGrpah;
		private:
			std::vector<Ref<FRGPassBase>> m_Passes;

			std::set<FRGTexture2DID> m_TextureCreates;
			std::set<FRGTexture2DID> m_TextureReads;
			std::set<FRGTexture2DID> m_TextureWrites;
			std::set<FRGTexture2DID> m_TextureDestroys;
			std::map<FRGTexture2DID, EResourceState> m_TextureStateMap;

			std::set<FRGBufferID> m_BufferCreates;
			std::set<FRGBufferID> m_BufferReads;
			std::set<FRGBufferID> m_BufferWrites;
			std::set<FRGBufferID> m_BufferDestroys;
			std::map<FRGBufferID, EResourceState> m_BufferStateMap;
		};
	public:
		FRenderGraph(FRGResourcePool& Pool);
		~FRenderGraph();
	

		template<typename PassData, typename... FArgs> requires std::is_constructible_v<FRederGraphPass<PassData>, FArgs...>
		decltype(auto) AddPass(FArgs&&... Args)
		{
			m_Passes.emplace_back(CreateScope<FRederGraphPass<PassData>>(std::forward<FArgs>(Args)...));
			FRenderGraphBuilder Builder(*this, *m_Passes.back());
			m_Passes.back()->Setup(Builder);
			return *dynamic_cast<FRederGraphPass<PassData>*>(m_Passes.back().get());
		}


		void DestroyTexture(FDependencyLevel& DependencyLevel);
		void DestroyBuffer(FDependencyLevel& DependencyLevel);

		void ImportTexture2D(FRGResourceName Name, FTexture2D* Texture);
		void ImportTextureCube(FRGResourceName Name, FTextureCube* TextureCube);
		void ImportBuffer(FRGResourceName Name, FBuffer* Buffer);

		void Build();
		void Execute();

		bool IsTexture2DDeclared(FRGResourceName ResourceName);
		bool IsBufferDeclared(FRGResourceName ResourceName);

		inline FRGTexture* GetRGTexture2D(FRGTexture2DID RGTextureID) const;
		FTexture2D* GetTexture2D(FRGTexture2DID RGTextureID);
		inline FRGBuffer* GetRGBuffer(FRGBufferID RGBufferID) const;
		FBuffer* GetBuffer(FRGBufferID RGBufferID);
		FRGTexture2DID GetTexture2DID(FRGResourceName Name);
		FRGBufferID GetBufferID(FRGResourceName Name);

		FRGResourcePool& GetResourcePool() { return m_ResourcePool; };

	private:
		FRGResourcePool& m_ResourcePool;
		std::vector<Ref<FRGPassBase>> m_Passes;
		std::vector<Scope<FRGTexture>> m_Texture2Ds;
		std::vector<Scope<FRGBuffer>> m_Buffers;

		std::vector<std::vector<size_t>> m_AdjacencyList;
		std::vector<size_t> m_TopologicallySortedPasses;
		std::vector<FDependencyLevel> m_DependencyLevels;


		std::map<FRGResourceName, FRGTexture2DID> m_Texture2DNameIDMap;
		std::map<FRGResourceName, FRGBufferID>  m_BufferNameIDMap;
		std::map<FRGBufferReadWriteID, FRGBufferID> m_BufferUavCounterMap;

		// mutable std::map<FRGTextureID, std::vector<std::pair<FTextureSubresourceDesc, ERGDescriptorType>>> m_TextureViewDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_SRVTexDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_DSVTexDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_RTVTexDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_UAVTexDescMap;
		
		//mutable std::map<FRGBufferID, std::vector<std::pair<FBufferSubresourceDesc, ERGDescriptorType>>> m_BufferViewDescMap;

		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_SRVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_DSVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_RTVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_UAVBufferDescMap;

	private:
		void AddBufferBindFlags(FRGResourceName Name,  EResourceBindFlag Flags);
		void AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags);

		void BuildAdjacencyLists();
		void TopologicalSort();
		void BuildDependencyLevels();
		void CullPasses();
		void CalculateResourcesLifetime();
		void DepthFirstSearch(size_t i, std::vector<bool>& visited, std::stack<size_t>& Stack);
		
		FRGTexture2DID DeclareTexture(FRGResourceName Name, const FRGTextureDesc& Desc);
		FRGBufferID DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc);
		

		bool IsValidTextureHandle(FRGTexture2DID RGTextureID) const;
		bool IsValidBufferHandle(FRGBufferID RGBufferID) const;
		
		void CreateTextureViews(FRGTexture2DID RGTextureID);
		void CreateBufferViews(FRGBufferID RGBufferID);
		
		


		FRGTexture2DCopySrcID ReadCopySrcTexture(FRGResourceName Name);
		FRGTexture2DCopyDstID WriteCopyDstTexture(FRGResourceName Name);
		
		FRGTex2DRenderTargetID RenderTarget(FRGResourceName name, const FTextureSubresourceDesc& desc);
		FRGTex2DDepthStencilID DepthStencil(FRGResourceName name, const FTextureSubresourceDesc& desc);
		FRGTex2DReadOnlyID ReadTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc); 
		FRGTex2DReadWriteID WriteTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGBufferReadOnlyID ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
	};
}