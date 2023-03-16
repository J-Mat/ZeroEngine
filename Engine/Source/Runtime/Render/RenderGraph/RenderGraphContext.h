#pragma once
#include "RenderGraphResourceID.h"
#include "Render/RHI/Texture.h"
#include "Render/RhI/Buffer/Buffer.h"

namespace Zero
{
	class FRGPassBase;
	class FRenderGraph;

	template<ERGResourceType ResourceType>
	struct FRGResourceTraits;
	
	template<>
	struct FRGResourceTraits<ERGResourceType::Texture>
	{
		using FResource = FTexture2D;
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
		FRGPassBase* LastUsedBy = nullptr;
	};

	template<ERGResourceType ResourceType>
	struct FTypedRGResource :  FRGResource
	{
		using FResource = FRGResourceTraits<ResourceType>::FResource;
		using FResourceDesc = FRGResourceTraits<ResourceType>::FResourceDesc;
		
		FTypedRGResource(uint32_t ID, Ref<FResource> _Resource, char const* Name = "")
			: FRGResource(ID, true, Name), Resource(_Resource), Desc(_Resource->GetDesc())
		{}

		FTypedRGResource(uint32_t ID, FResourceDesc const& _Desc, char const* Name = "")
			: FRGResource(ID, false, Name), Resource(nullptr), Desc(_Desc)
		{}
		Ref<FResource> Resource;
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

		Ref<FTexture2D> GetTexture(FRGTextureID ResID) const;
		Ref<FBuffer> GetBuffer(FRGBufferID ResID) const;
		FRenderGraphContext(FRenderGraph& RenderGraph, FRGPassBase& RGPass);
	private:
		FRenderGraph& m_RenderGrpah;
		FRGPassBase& m_RgPass;
	};
}
