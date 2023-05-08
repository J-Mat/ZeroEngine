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

			std::set<FRGTexture2DID> m_Texture2DCreates;
			std::set<FRGTexture2DID> m_Texture2DReads;
			std::set<FRGTexture2DID> m_Texture2DWrites;
			std::set<FRGTexture2DID> m_Texture2DDestroys;
			std::map<FRGTexture2DID, EResourceState> m_Texture2DStateMap;

			std::set<FRGTextureCubeID> m_TextureCubeCreates;
			std::set<FRGTextureCubeID> m_TextureCubeReads;
			std::set<FRGTextureCubeID> m_TextureCubeWrites;
			std::set<FRGTextureCubeID> m_TextureCubeDestroys;
			std::map<FRGTextureCubeID, EResourceState> m_TextureCubeStateMap;

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
		bool IsTextureCubeDeclared(FRGResourceName ResourceName);
		bool IsBufferDeclared(FRGResourceName ResourceName);

		inline FRGTexture2D* GetRGTexture2D(FRGTexture2DID RGTextureID) const;
		inline FRGTextureCube* GetRGTextureCube(FRGTextureCubeID RGTextureID) const;
		FTexture2D* GetTexture2D(FRGTexture2DID RGTextureID);
		FTextureCube* GetTextureCube(FRGTextureCubeID RGTextureID);
		inline FRGBuffer* GetRGBuffer(FRGBufferID RGBufferID) const;
		FBuffer* GetBuffer(FRGBufferID RGBufferID);
		FRGTexture2DID GetTexture2DID(FRGResourceName Name);
		FRGTextureCubeID GetTextureCubeID(FRGResourceName Name);
		FRGBufferID GetBufferID(FRGResourceName Name);

		FRGResourcePool& GetResourcePool() { return m_ResourcePool; };

	private:
		FRGResourcePool& m_ResourcePool;
		std::vector<Ref<FRGPassBase>> m_Passes;
		std::vector<Scope<FRGTexture2D>> m_Texture2Ds;
		std::vector<Scope<FRGTextureCube>> m_TextureCubes;
		std::vector<Scope<FRGBuffer>> m_Buffers;

		std::vector<std::vector<size_t>> m_AdjacencyList;
		std::vector<size_t> m_TopologicallySortedPasses;
		std::vector<FDependencyLevel> m_DependencyLevels;


		std::map<FRGResourceName, FRGTexture2DID> m_Texture2DNameIDMap;
		std::map<FRGResourceName, FRGTextureCubeID> m_TextureCubeNameIDMap;
		std::map<FRGResourceName, FRGBufferID>  m_BufferNameIDMap;
		std::map<FRGBufferReadWriteID, FRGBufferID> m_BufferUavCounterMap;

		// mutable std::map<FRGTexture2DID, std::vector<std::pair<FTextureSubresourceDesc, ERGDescriptorType>>> m_TextureViewDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_SRVTex2DDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_DSV2DTexDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_RTVTex2DDescMap;
		std::map<FRGTexture2DID, std::vector<FTextureSubresourceDesc>> m_UAVTex2DDescMap;
		
		std::map<FRGTextureCubeID, std::vector<FTextureSubresourceDesc>> m_SRVTexCubeDescMap;
		std::map<FRGTextureCubeID, std::vector<FTextureSubresourceDesc>> m_DSVTexCubeDescMap;
		std::map<FRGTextureCubeID, std::vector<FTextureSubresourceDesc>> m_RTVTexCubeDescMap;
		std::map<FRGTextureCubeID, std::vector<FTextureSubresourceDesc>> m_UAVTexCubeDescMap;
		//mutable std::map<FRGBufferID, std::vector<std::pair<FBufferSubresourceDesc, ERGDescriptorType>>> m_BufferViewDescMap;

		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_SRVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_DSVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_RTVBufferDescMap;
		std::map<FRGBufferID, std::vector<FBufferSubresourceDesc>> m_UAVBufferDescMap;

	private:
		void AddBufferBindFlags(FRGResourceName Name,  EResourceBindFlag Flags);
		void AddTexture2DBindFlags(FRGResourceName Name, EResourceBindFlag Flags);
		void AddTextureCubeBindFlags(FRGResourceName Name, EResourceBindFlag Flags);

		void BuildAdjacencyLists();
		void TopologicalSort();
		void BuildDependencyLevels();
		void CullPasses();
		void CalculateResourcesLifetime();
		void DepthFirstSearch(size_t i, std::vector<bool>& visited, std::stack<size_t>& Stack);
		
		FRGTexture2DID DeclareTexture2D(FRGResourceName Name, const FRGTextureDesc& Desc);
		FRGTextureCubeID DeclareTextureCube(FRGResourceName Name, const FRGTextureDesc& Desc);
		FRGBufferID DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc);
		

		bool IsValidTexture2DHandle(FRGTexture2DID RGTextureID) const;
		bool IsValidTextureCubeHandle(FRGTextureCubeID RGTextureID) const;
		bool IsValidBufferHandle(FRGBufferID RGBufferID) const;
		
		void CreateTexture2DViews(FRGTexture2DID RGTextureID);
		void CreateTextureCubeViews(FRGTextureCubeID RGTextureCubeID);
		void CreateBufferViews(FRGBufferID RGBufferID);
		
		


		FRGTexture2DCopySrcID ReadCopySrcTexture2D(FRGResourceName Name);
		FRGTexture2DCopyDstID WriteCopyDstTexture2D(FRGResourceName Name);
		
		FRGTex2DRenderTargetID RenderTarget2D(FRGResourceName name, const FTextureSubresourceDesc& desc);
		FRGTex2DDepthStencilID DepthStencil2D(FRGResourceName name, const FTextureSubresourceDesc& desc);
		FRGTex2DReadOnlyID ReadTexture2D(FRGResourceName Name, const FTextureSubresourceDesc& Desc); 
		FRGTex2DReadWriteID WriteTexture2D(FRGResourceName Name, const FTextureSubresourceDesc& Desc);

		FRGTexCubeRenderTargetID RenderTargetCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGTexCubeDepthStencilID DepthStencilCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGTexCubeReadOnlyID ReadTextureCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc); 
		FRGTexCubeReadWriteID WriteTextureCube(FRGResourceName Name, const FTextureSubresourceDesc& Desc);

		FRGBufferReadOnlyID ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
	};
}