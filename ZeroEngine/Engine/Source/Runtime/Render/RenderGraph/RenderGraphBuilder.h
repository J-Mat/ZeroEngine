#pragma once
#include "Core.h"
#include "RenderGraphResourceName.h"
#include "RenderGraphPass.h"
#include "RenderGraphContext.h"
#include "Render/ResourceCommon.h"

namespace Zero
{
	struct FRGTextureDesc
	{
		ETextureType Type = ETextureType::TextureType_2D;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Depth = 0;
		uint32_t ArraySize = 1;
		uint32_t MipLevels = 1;
		uint32_t SampleCount = 1;
		EResourceUsage HeapType = EResourceUsage::Default;
		FTextureClearValue ClearValue{};
		EResourceFormat Format = EResourceFormat::UNKNOWN;
	};
	struct FRGBufferDesc
	{
		uint64_t Size = 0;
		uint32_t Stride = 0;
		EResourceUsage ResourceUsage = EResourceUsage::Default;
		EBufferMiscFlag MiscFlags = EBufferMiscFlag::None;
		EResourceFormat Format = EResourceFormat::UNKNOWN;
	};

	static void FillTextureDesc(FRGTextureDesc const& RGTexDesc, FTextureDesc& TexDesc)
	{
		TexDesc.Type = RGTexDesc.Type;
		TexDesc.Width = RGTexDesc.Width;
		TexDesc.Height = RGTexDesc.Height;
		TexDesc.Depth = RGTexDesc.Depth;
		TexDesc.ArraySize = RGTexDesc.ArraySize;
		TexDesc.MipLevels = RGTexDesc.MipLevels;
		TexDesc.Format = RGTexDesc.Format;
		TexDesc.SampleCount = RGTexDesc.SampleCount;
		TexDesc.HeapType = RGTexDesc.HeapType;
		TexDesc.ClearValue = RGTexDesc.ClearValue;
		TexDesc.ResourceBindFlags = EResourceBindFlag::None;
		TexDesc.InitialState = EResourceState::Common;
	}

	static void FillBufferDesc(FRGBufferDesc const& RGBufferDesc, FBufferDesc& BufferDesc)
	{
		BufferDesc.Size = RGBufferDesc.Size;
		BufferDesc.Stride = RGBufferDesc.Stride;
		BufferDesc.ResourceUsage = RGBufferDesc.ResourceUsage;
		BufferDesc.BufferMiscFlag = RGBufferDesc.MiscFlags;
		BufferDesc.Format = RGBufferDesc.Format;
		BufferDesc.ResourceBindFlag = EResourceBindFlag::None;
	}

	class FRenderGraphBuilder
	{
		friend class FRenderGraph;
	public:
		FRenderGraphBuilder() = delete;
		FRenderGraphBuilder(FRenderGraphBuilder const&) = delete;
		FRenderGraphBuilder& operator=(FRenderGraphBuilder const&) = delete;


		bool IsTextureDeclared(FRGResourceName Name);
		bool IsBufferDeclared(FRGResourceName Name);
		void DeclareTexture(FRGResourceName Name, const FRGTextureDesc& desc);
		void DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& desc);
		

		void DummyWriteTexture(FRGResourceName Name);
		void DummyReadTexture(FRGResourceName Name);
		void DummyReadBuffer(FRGResourceName Name);
		void DummyWriteBuffer(FRGResourceName Name);

		FRGTextureReadOnlyID ReadTexture(FRGResourceName Name, ERGReadAccess RGReadAccess = ERGReadAccess::ReadAccess_AllShader,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			return ReadTextureImpl(Name, RGReadAccess,
				FTextureSubresourceDesc{
					.FirstSlice = FirstSlice,
					.SliceCount = SliceCount,
					.FirstMip = FirstMip,
					.MipCount = MipCount
				}
			);
		}

		FRGTextureReadWriteID WriteTexture(FRGResourceName Name,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			return WriteTextureImpl(Name,
				FTextureSubresourceDesc{
					.FirstSlice = FirstSlice,
					.SliceCount = SliceCount,
					.FirstMip = FirstMip,
					.MipCount = MipCount
				}
			);
		}

		FRGRenderTargetID WriteRenderTarget(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			return WriteRenderTargetImpl(Name, LoadStoreAccessOp,
				FTextureSubresourceDesc{
					.FirstSlice = FirstSlice,
					.SliceCount = SliceCount,
					.FirstMip = FirstMip,
					.MipCount = MipCount
				}
			);
		}

		FRGDepthStencilID WriteDepthStencil(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			return WriteDepthStencilImpl(Name, LoadStoreAccessOp, ERGLoadStoreAccessOp::NoAccess_NoAccess,
				FTextureSubresourceDesc{
					.FirstSlice = FirstSlice,
					.SliceCount = SliceCount,
					.FirstMip = FirstMip,
					.MipCount = MipCount
				}
			);
		}
		FRGDepthStencilID ReadDepthStencil(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp, ERGLoadStoreAccessOp StencilLoadStoreOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			return ReadDepthStencilImpl(Name, LoadStoreAccessOp, StencilLoadStoreOp,
				FTextureSubresourceDesc{
					.FirstSlice = FirstSlice,
					.SliceCount = SliceCount,
					.FirstMip = FirstMip,
					.MipCount = MipCount
				}
			);
		}
		
		FRGBufferReadOnlyID ReadBuffer(FRGResourceName Name, ERGReadAccess ReadAccess = ERGReadAccess::ReadAccess_AllShader, uint32_t Offset = 0, uint32_t Size = -1)
		{
			return ReadBufferImpl(Name, ReadAccess, 
				FBufferSubresourceDesc{
					.Offset = Offset,
					.Size = Size
				}
			);
		}

		FRGBufferReadWriteID WriteBuffer(FRGResourceName Name, uint32_t Offset = 0, uint32_t Size = -1)
		{
			return WriteBufferImpl(Name,
				FBufferSubresourceDesc{
					.Offset = Offset,
					.Size = Size
				}
			);
		}
		
		void SetViewport(uint32_t Width, uint32_t Height);
		void AddBufferBindFlags(FRGResourceName Name, EResourceBindFlag Flags);
		void AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags);

	private:
		FRenderGraph& m_RenderGrpah;
		FRGPassBase& m_RgPass;
	private:
		FRenderGraphBuilder(FRenderGraph&, FRGPassBase&);

		FRGTextureReadOnlyID ReadTextureImpl(FRGResourceName Name, ERGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc);
		FRGTextureReadWriteID WriteTextureImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGRenderTargetID WriteRenderTargetImpl(FRGResourceName Name,  ERGLoadStoreAccessOp LoadStoreOp, const FTextureSubresourceDesc& Desc);
		FRGDepthStencilID WriteDepthStencilImpl(FRGResourceName Name,  ERGLoadStoreAccessOp LoadStoreOp,  ERGLoadStoreAccessOp StencilLoadStoreOp, const FTextureSubresourceDesc& Desc);
		FRGDepthStencilID ReadDepthStencilImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOps, const FTextureSubresourceDesc& Desc);
		FRGBufferReadOnlyID	ReadBufferImpl(FRGResourceName Name, ERGReadAccess ReadAccess, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBufferImpl(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBufferImpl(FRGResourceName Name, FRGResourceName CounterName, const FBufferSubresourceDesc& Desc);
	};
}