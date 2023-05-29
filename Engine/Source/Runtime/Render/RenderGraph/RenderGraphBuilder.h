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
		
		static FRGTextureDesc MakeShadow2DDesc(uint32_t Width, uint32_t Height)
		{
			FRGTextureDesc Desc = 
			{
				.Type = ETextureType::TextureType_2D,
				.Width = Width,
				.Height = Height,
				.ArraySize = 1,
				.ClearValue = FTextureClearValue(1.0f, 0),
				.Format = EResourceFormat::D24_UNORM_S8_UINT,
			};
			return Desc;
		}

		static FRGTextureDesc MakeShadowCubeDesc(uint32_t Width, uint32_t Height)
		{
			FRGTextureDesc Desc = 
			{
				.Type = ETextureType::TextureType_2D,
				.Width = Width,
				.Height = Height,
				.ArraySize = 6,
				.ClearValue = FTextureClearValue(1.0f, 0),
				.Format = EResourceFormat::D24_UNORM_S8_UINT,
			};
			return Desc;
		}

		static FRGTextureDesc MakeColorCubeDesc(uint32_t Width, uint32_t Height)
		{
			FRGTextureDesc Desc = 
			{
				.Type = ETextureType::TextureType_2D,
				.Width = Width,
				.Height = Height,
				.ArraySize = 6,
				.ClearValue = FTextureClearValue(0.0f, 0.0f, 0.0f, 0.0f),
				.Format = EResourceFormat::R8G8B8A8_UNORM,
			};
			return Desc;
		}
	};

	struct FRGBufferDesc
	{
		uint32_t Size = 0;
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


		bool IsTexture2DDeclared(FRGResourceName Name);
		bool IsTextureCubeDeclared(FRGResourceName Name);
		bool IsBufferDeclared(FRGResourceName Name);
		void DeclareTexture2D(FRGResourceName Name, const FRGTextureDesc& desc);
		void DeclareTextureCube(FRGResourceName Name, const FRGTextureDesc& desc);
		void DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& desc);
		

		void DummyWriteTexture2D(FRGResourceName Name);
		void DummyReadTexture2D(FRGResourceName Name);
		void DummyWriteTextureCube(FRGResourceName Name);
		void DummyReadTextureCube(FRGResourceName Name);
		void DummyReadBuffer(FRGResourceName Name);
		void DummyWriteBuffer(FRGResourceName Name);

		FRGTexture2DCopySrcID ReadCopySrcTexture(FRGResourceName Name);
		FRGTexture2DCopyDstID WriteCopyDstTexture(FRGResourceName Name);

		FRGTex2DReadOnlyID ReadTexture2D(FRGResourceName Name, ERGReadAccess RGReadAccess = ERGReadAccess::ReadAccess_AllShader,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return ReadTexture2DImpl(Name, RGReadAccess, Desc);
		}

		FRGTexCubeReadOnlyID ReadTextureCube(FRGResourceName Name, ERGReadAccess RGReadAccess = ERGReadAccess::ReadAccess_AllShader,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return ReadTextureCubeImpl(Name, RGReadAccess, Desc);
		}

		FRGTex2DReadWriteID WriteTexture2D(FRGResourceName Name,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			m_RgPass.m_RenderPassRTType = ERenderPassRTType::Texuture2D;
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return WriteTexture2DImpl(Name, Desc);
		}

		FRGTexCubeReadWriteID WriteTextureCube(FRGResourceName Name,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			m_RgPass.m_RenderPassRTType = ERenderPassRTType::TexutureCube;
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return WriteTextureCubeImpl(Name, Desc);
		}

		FRGTex2DRenderTargetID WriteRenderTarget2D(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			m_RgPass.m_RenderPassRTType = ERenderPassRTType::Texuture2D;
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return WriteRenderTarget2DImpl(Name, LoadStoreAccessOp, Desc);
		}

		FRGTexCubeRenderTargetID WriteRenderTargetCube(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			m_RgPass.m_RenderPassRTType = ERenderPassRTType::TexutureCube;
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return WriteRenderTargetCubeImpl(Name, LoadStoreAccessOp, Desc);
		}

		FRGTex2DDepthStencilID WriteDepthStencil2D(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			m_RgPass.m_RenderPassRTType = ERenderPassRTType::Texuture2D;
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip,
			Desc.MipCount = MipCount;
			return WriteDepthStencil2DImpl(Name, LoadStoreAccessOp, ERGLoadStoreAccessOp::NoAccess_NoAccess, Desc);
		}

		FRGTexCubeDepthStencilID WriteDepthStencilCube(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			m_RgPass.m_RenderPassRTType = ERenderPassRTType::TexutureCube;
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip,
			Desc.MipCount = MipCount;
			return WriteDepthStencilCubeImpl(Name, LoadStoreAccessOp, ERGLoadStoreAccessOp::NoAccess_NoAccess, Desc);
		}

		FRGTex2DDepthStencilID ReadDepthStencil2D(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp, ERGLoadStoreAccessOp StencilLoadStoreOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return ReadDepthStencil2DImpl(Name, LoadStoreAccessOp, StencilLoadStoreOp, Desc);
		}
		
		FRGTexCubeDepthStencilID ReadDepthStencilCube(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreAccessOp, ERGLoadStoreAccessOp StencilLoadStoreOp,
			uint32_t FirstMip = 0, uint32_t MipCount = -1, uint32_t FirstSlice = 0, uint32_t SliceCount = -1)
		{
			FTextureSubresourceDesc Desc;
			Desc.FirstSlice = FirstSlice;
			Desc.SliceCount = SliceCount;
			Desc.FirstMip = FirstMip;
			Desc.MipCount = MipCount;
			return ReadDepthStencilCubeImpl(Name, LoadStoreAccessOp, StencilLoadStoreOp, Desc);
		}

		FRGBufferReadOnlyID ReadBuffer(FRGResourceName Name, ERGReadAccess ReadAccess = ERGReadAccess::ReadAccess_AllShader, uint32_t Offset = 0, uint32_t Size = -1)
		{
			FBufferSubresourceDesc Desc;
			Desc.Offset = Offset;
			Desc.Size = Size;
			return ReadBufferImpl(Name, ReadAccess, Desc);
		}

		FRGBufferReadWriteID WriteBuffer(FRGResourceName Name, uint32_t Offset = 0, uint32_t Size = -1)
		{
			FBufferSubresourceDesc Desc;
			Desc.Offset = Offset;
			Desc.Size = Size;
			return WriteBufferImpl(Name, Desc);
		}
		
		void SetViewport(uint32_t Width, uint32_t Height, uint32_t Depth = 0);
		void AddBufferBindFlags(FRGResourceName Name, EResourceBindFlag Flags);
		void AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags);

	private:
		FRenderGraph& m_RenderGrpah;
		FRGPassBase& m_RgPass;
	private:
		FRenderGraphBuilder(FRenderGraph&, FRGPassBase&);

		FRGTex2DReadOnlyID ReadTexture2DImpl(FRGResourceName Name, ERGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc);
		FRGTex2DReadWriteID WriteTexture2DImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGTex2DRenderTargetID WriteRenderTarget2DImpl(FRGResourceName Name,  ERGLoadStoreAccessOp LoadStoreOp, const FTextureSubresourceDesc& Desc);
		FRGTex2DDepthStencilID WriteDepthStencil2DImpl(FRGResourceName Name,  ERGLoadStoreAccessOp LoadStoreOp,  ERGLoadStoreAccessOp StencilLoadStoreOp, const FTextureSubresourceDesc& Desc);
		FRGTex2DDepthStencilID ReadDepthStencil2DImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOps, const FTextureSubresourceDesc& Desc);

		FRGTexCubeReadOnlyID ReadTextureCubeImpl(FRGResourceName Name, ERGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc);
		FRGTexCubeReadWriteID WriteTextureCubeImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc);
		FRGTexCubeRenderTargetID WriteRenderTargetCubeImpl(FRGResourceName Name,  ERGLoadStoreAccessOp LoadStoreOp, const FTextureSubresourceDesc& Desc);
		FRGTexCubeDepthStencilID WriteDepthStencilCubeImpl(FRGResourceName Name,  ERGLoadStoreAccessOp LoadStoreOp,  ERGLoadStoreAccessOp StencilLoadStoreOp, const FTextureSubresourceDesc& Desc);
		FRGTexCubeDepthStencilID ReadDepthStencilCubeImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOps, const FTextureSubresourceDesc& Desc);

		FRGBufferReadOnlyID	ReadBufferImpl(FRGResourceName Name, ERGReadAccess ReadAccess, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBufferImpl(FRGResourceName Name, const FBufferSubresourceDesc& Desc);
		FRGBufferReadWriteID WriteBufferImpl(FRGResourceName Name, FRGResourceName CounterName, const FBufferSubresourceDesc& Desc);
	};
}