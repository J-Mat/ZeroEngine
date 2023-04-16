#include "RenderGraphBuilder.h"
#include "RenderGraph.h"

namespace Zero
{
	FRenderGraphBuilder::FRenderGraphBuilder(FRenderGraph& RenderGraph, FRGPassBase& RGPassBase)
		: m_RenderGrpah(RenderGraph), m_RgPass(RGPassBase)
	{

	}

	void FRenderGraphBuilder::SetViewport(uint32_t Width, uint32_t Height)
	{
		m_RgPass.m_VieportWidth = Width;
		m_RgPass.m_VieportHeight = Height;
	}

	void FRenderGraphBuilder::AddBufferBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
	{
		m_RenderGrpah.AddBufferBindFlags(Name, Flags);
	}

	void FRenderGraphBuilder::AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
	{
		m_RenderGrpah.AddTextureBindFlags(Name, Flags);
	}

	FRGTextureReadOnlyID FRenderGraphBuilder::ReadTextureImpl(FRGResourceName Name, ERGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTextureReadOnlyID RGTextureReadOnlyID = m_RenderGrpah.ReadTexture(Name, Desc);
		FRGTextureID RGTextureID = RGTextureReadOnlyID.GetResourceID();
		m_RgPass.m_TextureReads.insert(RGTextureID);
		return RGTextureReadOnlyID;
	}

	FRGTextureReadWriteID FRenderGraphBuilder::WriteTextureImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTextureReadWriteID RGTextureWriteOnlyID = m_RenderGrpah.WriteTexture(Name, Desc);
		FRGTextureID RGTextureID = RGTextureWriteOnlyID.GetResourceID();
		if (!m_RgPass.m_TextureCreates.contains(RGTextureID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture(Name);
		}
		m_RgPass.m_TextureWrites.insert(RGTextureID);
		FRGTexture* Texture = m_RenderGrpah.GetRGTexture(RGTextureID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGTextureWriteOnlyID;
	}

	FRGRenderTargetID FRenderGraphBuilder::WriteRenderTargetImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGRenderTargetID RenderTargetID = m_RenderGrpah.RenderTarget(Name, Desc);
		FRGTextureID ResID = RenderTargetID.GetResourceID();
		FRGPassBase::FRenderTargetInfo RenderTargetInfo
		{
			.RGRenderTargetID = RenderTargetID,
			.RenderTargetAccess = LoadStoreOp,
		};
		m_RgPass.m_RenderTergetInfo.push_back(RenderTargetInfo);
		if (!m_RgPass.m_TextureCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture(Name);
		}
		m_RgPass.m_TextureWrites.insert(m_RenderGrpah.GetTextureID(Name));
		FRGTexture* Texture = m_RenderGrpah.GetRGTexture(ResID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RenderTargetID;
	}

	FRGDepthStencilID FRenderGraphBuilder::WriteDepthStencilImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOp, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGDepthStencilID DepthStencilID = m_RenderGrpah.DepthStencil(Name, Desc);
		FRGTextureID ResID = DepthStencilID.GetResourceID();
		FRGPassBase::FDepthStencilInfo DepthStencilInfo
		{
			.RGDepthStencilID = DepthStencilID,
			.DepthAccess = LoadStoreOp,
			.StencilAccess = StencilLoadStoreOp,
			.bReadOnly = false
		};
		m_RgPass.m_DepthStencil = DepthStencilInfo;
		if (!m_RgPass.m_TextureCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture(Name);
		}
		m_RgPass.m_TextureWrites.insert(m_RenderGrpah.GetTextureID(Name));
		FRGTexture* Texture = m_RenderGrpah.GetRGTexture(ResID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return DepthStencilID;
	}

	FRGDepthStencilID FRenderGraphBuilder::ReadDepthStencilImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOps, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGDepthStencilID RGDepthStencilID = m_RenderGrpah.DepthStencil(Name, Desc);
		FRGTextureID ResID = RGDepthStencilID.GetResourceID();
		FRGPassBase::FDepthStencilInfo DepthStencilInfo
		{
			.RGDepthStencilID = RGDepthStencilID,
			.DepthAccess = LoadStoreOp,
			.StencilAccess = StencilLoadStoreOps,
			.bReadOnly = false
		};
		FRGTexture* Texture = m_RenderGrpah.GetRGTexture(ResID);
		m_RgPass.m_TextureReads.insert(ResID);


		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGDepthStencilID;
	}
	

	FRGBufferReadOnlyID FRenderGraphBuilder::ReadBufferImpl(FRGResourceName Name, ERGReadAccess ReadAccess, const FBufferSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGBufferReadOnlyID ReadOnlyID = m_RenderGrpah.ReadBuffer(Name, Desc);
		if (m_RgPass.GetPassType() == ERenderPassType::Compute)
		{
			ReadAccess = ERGReadAccess::ReadAccess_NonPixelShader;
		}
		FRGBufferID ResID = ReadOnlyID.GetResourceID();
		m_RgPass.m_BufferCreates.insert(ResID);
		return ReadOnlyID;
	}

	FRGBufferReadWriteID FRenderGraphBuilder::WriteBufferImpl(FRGResourceName Name, const FBufferSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGBufferReadWriteID RGBufferReadWriteID = m_RenderGrpah.WriteBuffer(Name, Desc);
		FRGBufferID RGBufferID = RGBufferReadWriteID.GetResourceID();
		if (!m_RgPass.m_BufferCreates.contains(RGBufferID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadBuffer(Name);
		}
		m_RgPass.m_BufferWrites.insert(RGBufferID);
		FRGBuffer* RGBuffer = m_RenderGrpah.GetRGBuffer(RGBufferID);
		if (RGBuffer->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGBufferReadWriteID;
	}

	FRGBufferReadWriteID FRenderGraphBuilder::WriteBufferImpl(FRGResourceName Name, FRGResourceName CounterName, const FBufferSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGBufferReadWriteID RGBufferReadWriteID = m_RenderGrpah.WriteBuffer(Name, Desc);
		FRGBufferID CounterID = m_RenderGrpah.GetBufferID(CounterName);
		FRGBufferID ResID = RGBufferReadWriteID.GetResourceID();

		DummyWriteBuffer(CounterName);
		if (!m_RgPass.m_BufferCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadBuffer(Name);
			DummyReadBuffer(CounterName);
		}
		m_RgPass.m_BufferWrites.insert(ResID);
		FRGBuffer* RGBuffer = m_RenderGrpah.GetRGBuffer(ResID);
		if (RGBuffer->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGBufferReadWriteID;
	}

	void FRenderGraphBuilder::DummyWriteTexture(FRGResourceName Name)
	{
		m_RgPass.m_TextureWrites.insert(m_RenderGrpah.GetTextureID(Name));
	}

	void FRenderGraphBuilder::DummyReadTexture(FRGResourceName Name)
	{
		m_RgPass.m_TextureReads.insert(m_RenderGrpah.GetTextureID(Name));
	}

	void FRenderGraphBuilder::DummyReadBuffer(FRGResourceName Name)
	{
		m_RgPass.m_BufferReads.insert(m_RenderGrpah.GetBufferID(Name));
	}

	void FRenderGraphBuilder::DummyWriteBuffer(FRGResourceName Name)
	{
		m_RgPass.m_BufferWrites.insert(m_RenderGrpah.GetBufferID(Name));
	}

	FRGTextureCopySrcID FRenderGraphBuilder::ReadCopySrcTexture(FRGResourceName Name)
	{
		FRGTextureCopySrcID RGTextureCopySrcID = m_RenderGrpah.ReadCopySrcTexture(Name);
		FRGTextureID RGTextureID(RGTextureCopySrcID);
		m_RgPass.m_TextureReads.insert(RGTextureID);
		return RGTextureCopySrcID;
	}

	FRGTextureCopyDstID FRenderGraphBuilder::WriteCopyDstTexture(FRGResourceName Name)
	{
		FRGTextureCopyDstID RGTextureCopyDstID = m_RenderGrpah.WriteCopyDstTexture(Name);
		FRGTextureID RGTextureID(RGTextureCopyDstID);
		if (!m_RgPass.m_TextureCreates.contains(RGTextureID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture(Name);
		}
		m_RgPass.m_TextureWrites.insert(RGTextureID);
		auto* texture = m_RenderGrpah.GetRGTexture(RGTextureID);
		if (texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGTextureCopyDstID;
	}

	bool FRenderGraphBuilder::IsTextureDeclared(FRGResourceName Name)
	{
		return m_RenderGrpah.IsTextureDeclared(Name);
	}

	bool FRenderGraphBuilder::IsBufferDeclared(FRGResourceName Name)
	{
		return m_RenderGrpah.IsBufferDeclared(Name);
	}

	void FRenderGraphBuilder::DeclareTexture(FRGResourceName Name, const FRGTextureDesc& Desc)
	{
		m_RgPass.m_TextureCreates.insert(m_RenderGrpah.DeclareTexture(Name, Desc));
	}

	void FRenderGraphBuilder::DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc)
	{
		m_RgPass.m_BufferCreates.insert(m_RenderGrpah.DeclareBuffer(Name, Desc));
	}
}
