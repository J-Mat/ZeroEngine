#pragma once
#include "RenderGraphResourceID.h"
#include "Render/RHI/Texture.h"
#include "Render/RhI/Buffer.h"

namespace Zero
{
	class FRGPassBase;
	class FRenderGraph;

	template<ERGResourceType ResourceType>
	struct FRGResourceTraits;
	
	template<>
	struct FRGResourceTraits<ERGResourceType::Texture2D>
	{
		using FResource = FTexture2D;
		using FResourceDesc = FTextureDesc;
	};
	
	template<>
	struct FRGResourceTraits<ERGResourceType::TextureCube>
	{
		using FResource = FTextureCube;
		using FResourceDesc = FTextureDesc;
	};

	template<>
	struct FRGResourceTraits<ERGResourceType::Buffer>
	{
		using FResource = FBuffer;
		using FResourceDesc = FBufferDesc;
	};

	struct FRGResource
	{
		FRGResource(uint32_t _ID, bool _bImported, char const* _Name)
		: ID(_ID), bImported(_bImported), RefCount(0), Name(_Name)
		{}

		uint32_t ID;
		bool bImported;
		uint32_t RefCount;
		char const* Name;

		FRGPassBase* Writer = nullptr;
		FRGPassBase* LastPassUsedBy = nullptr;
	};

	template<ERGResourceType ResourceType>
	struct FTypedRGResource :  FRGResource
	{
		using FResource = FRGResourceTraits<ResourceType>::FResource;
		using FResourceDesc = FRGResourceTraits<ResourceType>::FResourceDesc;
		
		FTypedRGResource(uint32_t ID, FResource* _Resource, char const* Name = "")
			: FRGResource(ID, true, Name), Resource(_Resource), Desc(_Resource->GetDesc())
		{}

		FTypedRGResource(uint32_t ID, FResourceDesc const& _Desc, char const* Name = "")
			: FRGResource(ID, false, Name), Resource(nullptr), Desc(_Desc)
		{}
		FResource* Resource;
		FResourceDesc Desc;

		virtual ~FTypedRGResource() 
		{
			Resource = nullptr;
		}
	};
	using FRGTexture2D = FTypedRGResource<ERGResourceType::Texture2D>;
	using FRGTextureCube = FTypedRGResource<ERGResourceType::TextureCube>;
	using FRGBuffer = FTypedRGResource<ERGResourceType::Buffer>;	
	
	class FRenderGraphContext
	{
	public:
		FRenderGraphContext() = delete;
		FRenderGraphContext(FRenderGraphContext const&) = delete;
		FRenderGraphContext& operator=(FRenderGraphContext const&) = delete;

		FTexture2D* GetTexture2D(FRGTexture2DID ResID) const;
		FTextureCube* GetTextureCube(FRGTextureCubeID ResID) const;
		FBuffer* GetBuffer(FRGBufferID ResID) const;
		FRenderGraphContext(FRenderGraph& RenderGraph, FRGPassBase& RGPass);
	private:
		FRenderGraph& m_RenderGrpah;
		FRGPassBase& m_RgPass;
	};
}
