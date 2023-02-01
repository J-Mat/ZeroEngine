#pragma once
#include "RenderGraphResourceID.h"
#include "Render/RHI/Texture.h"
#include "Render/RhI/Buffer/Buffer.h"

namespace Zero
{
	class FRenderGraphPassBase;
	class FRenderGraph;

	template<ERGResourceType ResourceType>
	struct FRGResourceTraits;
	
	template<>
	struct FRGResourceTraits<ERGResourceType::Texture>
	{
		using Resourece = FTexture2D;
		using ResoureDesc = FTextureDesc;
	};
	
	template<>
	struct FRGResourceTraits<ERGResourceType::Buffer>
	{
		using Resource = FBuffer;
		using ResoureDesc = FBufferDesc;
	};

	struct FRGResource
	{
		FRGResource(uint32_t _ID, bool _bImported, char const* _Name)
		: ID(_ID), bImported(_bImported), RefCount(0), Name(_Name), 
		{}

		uint32_t ID;
		bool bImported;
		uint32_t RefCount;
		char const* Name;

		FRenderGraphPassBase* Writer = nullptr;
		FRenderGraphPassBase* LastUsedBy = nullptr;
	};

	template<ERGResourceType ResourceType>
	struct FTypedRGResource :  FRGResource
	{
		using FResource = FRGResourceTraits<ResourceType>::Resourece;
		using FResourceDesc = FRGResourceTraits<ResourceType>::ResoureDesc;
		
		FTypedRGResource(size_t ID, Resource* _Resource, char const* Name = "")
			: FRGResource(ID, true, Name), Resource(_Resource), desc(_Resource->GetDesc())
		{}

		FTypedRGResource(size_t ID, ResourceDesc const& _Desc, char const* Name = "")
			: FRGResource(ID, false, Name), Resource(nullptr), Desc(_Desc)
		{}
		FResource* Resource;
		FResourceDesc Desc;
	};
	using FRGTexture = FTypedRGResource<ERGResourceType::Texture>;
	using FRGBuffer = FTypedRGResource<ERGResourceType::Buffer>;	
	
	class FRenderGraphContext
	{
	public:
		FRenderGraphContext() = delete;
		FRenderGraphContext(FRenderGraphContext const&) = delete;
		FRenderGraphContext& operator=(FRenderGraphContext const&) = delete;

		FTexture2D const& GetTexture(FRGTextureID ResID) const;
		FBuffer  const& GetBuffer(FRGBufferID ResID) const;
	};
}
