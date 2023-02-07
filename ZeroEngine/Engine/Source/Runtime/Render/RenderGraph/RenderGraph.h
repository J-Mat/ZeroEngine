#pragma once
#include "Core.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphResourcePool.h"
#include "Render/ResourceCommon.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/Buffer/Buffer.h"

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
			void Execute();
			uint32_t GetSize();
			uint32_t GetNonCulledSize() const;
		private:
			FRenderGraph& m_RenderGrpah;
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
	private:
		FRGResourcePool m_ResourcePool;
		std::vector<Ref<FRGPassBase>> m_Passes;
		std::vector<Ref<FRGTexture>> m_Textures;
		std::vector<Ref<FRGBuffer>> m_Buffers;

		std::vector<std::vector<size_t>> m_AdjacencyList;
		std::vector<size_t> m_TopologicallySortedPasses;
		std::vector<FDependencyLevel> m_DependencyLevels;


		std::map<FRGResourceName, FRGTextureID> m_TextureNameIDMap;
		std::map<FRGResourceName, FRGBufferID>  m_BufferNameIDMap;
		std::map<FRGBufferReadWriteID, FRGBufferID> m_BufferUavCounterMap;

		mutable std::map<FRGTextureID, std::vector<std::pair<FTextureSubresourceDesc, ERGDescriptorType>>> m_TextureViewDescMap;
		mutable std::map<FRGTextureID, std::vector<std::pair<FResourceCpuHandle, ERGDescriptorType>>> m_TextureViewMap;
		
		mutable std::map<FRGBufferID, std::vector<std::pair<FBufferSubresourceDesc, ERGDescriptorType>>> m_BufferViewDescMap;
		mutable std::map<FRGBufferID, std::vector<std::pair<FResourceCpuHandle, ERGDescriptorType>>> m_BufferViewMap;

	private:
		FRGTextureID GetTextureId(FRGResourceName Name);
		FRGBufferID GetBufferId(FRGResourceName Name);

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
		
		void ImportTexture(FRGResourceName Name, Ref<FTexture2D> Texture);
		void ImportBuffer(FRGResourceName Name, Ref<FBuffer> Buffer);
		
		inline Ref<FRGTexture> GetRGTexture(FRGTextureID RGTextureID) const;
		Ref<FTexture2D> GetTexture(FRGTextureID RGTextureID);
		inline Ref<FRGBuffer> GetRGBuffer(FRGBufferID RGBufferID) const;
		Ref<FBuffer> GetBuffer(FRGBufferID RGBufferID);
		
		FRGTextureReadOnlyID ReadTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc); 
		FRGTextureReadWriteID WriteTexture(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGBufferReadOnlyID ReadBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBuffer(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
	
		FResourceCpuHandle GetRenderTarget(FRGRenderTargetID ID) const;
		FResourceCpuHandle GetDepthStencil(FRGRenderTargetID ID) const;
		FResourceCpuHandle GetReadOnlyTexture(FRGTextureReadOnlyID ID) const;
		FResourceCpuHandle GetReadWriteTexture(FRGTextureReadWriteID ID) const;
		FResourceCpuHandle GetReadOnlyBuffer(FRGBufferReadOnlyID ID) const;
		FResourceCpuHandle GetReadWriteBuffer(FRGBufferReadWriteID ID) const;
	};
}