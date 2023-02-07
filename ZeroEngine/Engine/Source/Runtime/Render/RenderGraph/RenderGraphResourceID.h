#pragma once
#include "Core.h"

namespace Zero
{
	enum class ERGResourceType : uint8_t
	{
		Buffer,
		Texture
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
		uint32_t ID;
	};
	

	template<ERGResourceType ResourceType>
	struct FTypedRGResourceId : FRGResourceID
	{
		using FRGResourceID::FRGResourceID;
	};

	using FRGBufferID = FTypedRGResourceId<ERGResourceType::Buffer>;
	using FRGTextureID = FTypedRGResourceId<ERGResourceType::Texture>;

	template<ERGResourceMode Mode>
	struct FRGTextureModeID : FRGTextureID
	{
		using RGTextureID::FRGTextureID;
	private:
		friend class FRenderGraphBuilder;
		friend class FRenderGraph;

		FRGTextureModeID(FRGTextureID const& ID) : FRGTextureID(ID) {}
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

	using FRGTextureCopySrcID = FRGTextureModeID<ERGResourceMode::CopySrc>;
	using FRGTextureCopyDstID = FRGTextureModeID<ERGResourceMode::CopyDst>;

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
		FRGResourceDescriptorID(uint32_t ViewID, FRGResourceID ResourceHandle) 
			: ID(InvalidID) 
		{
			ID = (ViewID << 32) | ResourceHandle.ID;
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
		RenderTarget,
		DepthStencil
	};

	template<ERGResourceType ResourceType, ERGDescriptorType ResourceViewType>
	struct FTypedRGResourceDescriptorId : FRGResourceDescriptorID
	{
		using FRGResourceDescriptorID::FRGResourceDescriptorID;
		
		auto GetResourceID() const
		{
			if constexpr (ResourceType == ERGResourceType::Buffer)
			{
				return FRGBufferID(FRGResourceDescriptorID::GetResourceID());
			}
			else if constexpr(ResourceType == ERGResourceType::Texture)
			{
				return FRGTextureID(FRGResourceDescriptorID::GetResourceID());
			}
		}
	};

	using FRGRenderTargetID = FTypedRGResourceDescriptorId<ERGResourceType::Texture, ERGDescriptorType::RenderTarget>;
	using FRGDepthStencilID = FTypedRGResourceDescriptorId<ERGResourceType::Texture, ERGDescriptorType::DepthStencil>;
	using FRGTextureReadOnlyID = FTypedRGResourceDescriptorId<ERGResourceType::Texture, ERGDescriptorType::ReadOnly>;
	using FRGTextureReadWriteID = FTypedRGResourceDescriptorId<ERGResourceType::Texture, ERGDescriptorType::ReadWrite>;

	using FRGBufferReadOnlyID = FTypedRGResourceDescriptorId<ERGResourceType::Buffer, ERGDescriptorType::ReadOnly>;
	using FRGBufferReadWriteID = FTypedRGResourceDescriptorId<ERGResourceType::Buffer, ERGDescriptorType::ReadWrite>;
}

namespace std
{
	template <> struct hash<Zero::FRGTextureID>
	{
		size_t operator()(Zero::FRGTextureID const& h) const
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
	template <> struct hash<Zero::FRGTextureReadOnlyID>
	{
		size_t operator()(Zero::FRGTextureReadOnlyID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGTextureReadWriteID>
	{
		size_t operator()(Zero::FRGTextureReadWriteID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGRenderTargetID>
	{
		size_t operator()(Zero::FRGRenderTargetID const& h) const
		{
			return hash<decltype(h.ID)>()(h.ID);
		}
	};
	template <> struct hash<Zero::FRGDepthStencilID>
	{
		size_t operator()(Zero::FRGDepthStencilID const& h) const
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

