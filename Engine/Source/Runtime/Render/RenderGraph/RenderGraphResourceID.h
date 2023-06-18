#pragma once
#include "Core.h"

namespace Zero
{
	enum class ERGResourceType : uint8_t
	{
		Buffer,
		Texture2D,
		TextureCube,
	};

	enum class ERGResourceMode : uint8_t
	{
		CopySrc,
		CopyDst,
		IndirectArgs,
		Vertex,
		Index,
		Constant
	};

	struct FRGResourceID
	{
		inline constexpr static uint32_t InvalidID = uint32_t(-1);
		FRGResourceID() : ID(InvalidID) {}
		FRGResourceID(FRGResourceID const&) = default;
		explicit FRGResourceID(uint32_t _ID) : ID(static_cast<uint32_t>(_ID)) {}

		void Invalidate() { ID = InvalidID; }
		bool IsValid() const { return ID != InvalidID;}
		auto operator<=>(FRGResourceID const&) const = default;
		uint32_t ID;
	};
	

	template<ERGResourceType ResourceType>
	struct FTypedRGResourceId : FRGResourceID
	{
		using FRGResourceID::FRGResourceID;
	};

	using FRGBufferID = FTypedRGResourceId<ERGResourceType::Buffer>;
	using FRGTexture2DID = FTypedRGResourceId<ERGResourceType::Texture2D>;
	using FRGTextureCubeID = FTypedRGResourceId<ERGResourceType::TextureCube>;

	template<ERGResourceMode Mode>
	struct FRGTextureMode2DID : FRGTexture2DID
	{
		using FRGTexture2DID::FRGTexture2DID;
	private:
		friend class FRenderGraphBuilder;
		friend class FRenderGraph;

		FRGTextureMode2DID(FRGTexture2DID const& ID) : FRGTexture2DID(ID) {}
	};

	template<ERGResourceMode Mode>
	struct FRGTextureModeCubeID : FRGTextureCubeID
	{
		using FRGTexture2DID::FRGTexture2DID;
	private:
		friend class FRenderGraphBuilder;
		friend class FRenderGraph;

		FRGTextureModeCubeID(FRGTextureCubeID const& ID) : FRGTextureCubeID(ID) {}
	};

	template<ERGResourceMode Mode>
	struct FRGBufferModeID : FRGBufferID
	{
		using FRGBufferID::FRGBufferID;
	private:
		friend class RenderGraphBuilder;
		friend class RenderGraph;

		FRGBufferModeID(FRGBufferID const& id) : FRGBufferID(id) {}
	};

	using FRGTexture2DCopySrcID = FRGTextureMode2DID<ERGResourceMode::CopySrc>;
	using FRGTexture2DCopyDstID = FRGTextureMode2DID<ERGResourceMode::CopyDst>;

	using FRGBufferCopySrcID = FRGBufferModeID<ERGResourceMode::CopySrc>;
	using FRGBufferCopyDstID = FRGBufferModeID<ERGResourceMode::CopyDst>;
	using FRGBufferIndirectArgsID = FRGBufferModeID<ERGResourceMode::IndirectArgs>;
	using FRGBufferVertexID = FRGBufferModeID<ERGResourceMode::Vertex>;
	using FRGBufferIndexID = FRGBufferModeID<ERGResourceMode::Index>;
	using FRGBufferConstantID = FRGBufferModeID<ERGResourceMode::Constant>;
		
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	struct FRGResourceDescriptorID
	{
		inline constexpr static uint64_t InvalidID = uint64_t(-1);
		FRGResourceDescriptorID() : ID(InvalidID) {}
		FRGResourceDescriptorID(uint32_t ViewID, FRGResourceID ResourceHandle) 
			: ID(InvalidID) 
		{
			ID = ((uint64_t)ViewID << 32) | ResourceHandle.ID;
		}
		uint32_t GetViewID() const { return ID >> 32; }
		uint32_t GetResourceID() const { return static_cast<uint32_t>(ID);};

		void Invalidate() { ID = InvalidID; }
		bool IsValid() const { return ID != InvalidID; }
		auto operator<=>(FRGResourceDescriptorID const&) const = default;
		uint64_t ID;
	};

	enum class ERGDescriptorType : uint8_t
	{
		ReadOnly,
		ReadWrite,
		ReadBack,
		RenderTarget,
		DepthStencil
	};

	template<ERGResourceType ResourceType, ERGDescriptorType ResourceViewType>
	struct FTypedRGResourceDescriptorID : FRGResourceDescriptorID
	{
		using FRGResourceDescriptorID::FRGResourceDescriptorID;
		
		auto GetResourceID() const
		{
			if constexpr (ResourceType == ERGResourceType::Buffer)
			{
				return FRGBufferID(FRGResourceDescriptorID::GetResourceID());
			}
			else if constexpr(ResourceType == ERGResourceType::Texture2D)
			{
				return FRGTexture2DID(FRGResourceDescriptorID::GetResourceID());
			}
			else if constexpr (ResourceType == ERGResourceType::TextureCube)
			{
				return FRGTextureCubeID(FRGResourceDescriptorID::GetResourceID());
			}
		}
	};

	using FRGTex2DRenderTargetID = FTypedRGResourceDescriptorID<ERGResourceType::Texture2D, ERGDescriptorType::RenderTarget>;
	using FRGTex2DDepthStencilID = FTypedRGResourceDescriptorID<ERGResourceType::Texture2D, ERGDescriptorType::DepthStencil>;
	using FRGTex2DReadOnlyID = FTypedRGResourceDescriptorID<ERGResourceType::Texture2D, ERGDescriptorType::ReadOnly>;
	using FRGTex2DReadWriteID = FTypedRGResourceDescriptorID<ERGResourceType::Texture2D, ERGDescriptorType::ReadWrite>;

	using FRGTexCubeRenderTargetID = FTypedRGResourceDescriptorID<ERGResourceType::TextureCube, ERGDescriptorType::RenderTarget>;
	using FRGTexCubeDepthStencilID = FTypedRGResourceDescriptorID<ERGResourceType::TextureCube, ERGDescriptorType::DepthStencil>;
	using FRGTexCubeReadOnlyID = FTypedRGResourceDescriptorID<ERGResourceType::TextureCube, ERGDescriptorType::ReadOnly>;
	using FRGTexCubeReadWriteID = FTypedRGResourceDescriptorID<ERGResourceType::TextureCube, ERGDescriptorType::ReadWrite>;

	using FRGBufferReadOnlyID = FTypedRGResourceDescriptorID<ERGResourceType::Buffer, ERGDescriptorType::ReadOnly>;
	using FRGBufferReadWriteID = FTypedRGResourceDescriptorID<ERGResourceType::Buffer, ERGDescriptorType::ReadWrite>;
}

namespace std
{
	template <> struct hash<Zero::FRGTexture2DID>
	{
		size_t operator()(Zero::FRGTexture2DID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};

	template <> struct hash<Zero::FRGTextureCubeID>
	{
		size_t operator()(Zero::FRGTextureCubeID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGBufferID>
	{
		size_t operator()(Zero::FRGBufferID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTex2DReadOnlyID>
	{
		size_t operator()(Zero::FRGTex2DReadOnlyID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTexCubeReadOnlyID>
	{
		size_t operator()(Zero::FRGTexCubeReadOnlyID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTex2DReadWriteID>
	{
		size_t operator()(Zero::FRGTex2DReadWriteID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTexCubeReadWriteID>
	{
		size_t operator()(Zero::FRGTexCubeReadWriteID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTex2DRenderTargetID>
	{
		size_t operator()(Zero::FRGTex2DRenderTargetID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTexCubeRenderTargetID>
	{
		size_t operator()(Zero::FRGTexCubeRenderTargetID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTex2DDepthStencilID>
	{
		size_t operator()(Zero::FRGTex2DDepthStencilID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTexCubeDepthStencilID>
	{
		size_t operator()(Zero::FRGTexCubeDepthStencilID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGBufferReadOnlyID>
	{
		size_t operator()(Zero::FRGBufferReadOnlyID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};

	template <> struct hash<Zero::FRGBufferReadWriteID>
	{
		size_t operator()(Zero::FRGBufferReadWriteID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
}

