#include "RenderGraphBuilder.h"
#include "RenderGraph.h"

namespace Zero
{
	FRenderGraphBuilder::FRenderGraphBuilder(FRenderGraph& RenderGraph, FRGPassBase& RGPassBase)
		: m_RenderGrpah(RenderGraph), m_RgPass(RGPassBase)
	{

	}

	void FRenderGraphBuilder::SetViewport(uint32_t Width, uint32_t Height, uint32_t Depth /*= 0*/)
	{
		m_RgPass.m_VieportWidth = Width;
		m_RgPass.m_VieportHeight = Height;
		m_RgPass.m_VieportDepth = Depth;
	}


	void FRenderGraphBuilder::AddBufferBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
	{
		m_RenderGrpah.AddBufferBindFlags(Name, Flags);
	}

	void FRenderGraphBuilder::AddTextureBindFlags(FRGResourceName Name, EResourceBindFlag Flags)
	{
		m_RenderGrpah.AddTexture2DBindFlags(Name, Flags);
	}

	FRGTex2DReadOnlyID FRenderGraphBuilder::ReadTexture2DImpl(FRGResourceName Name, ERGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTex2DReadOnlyID RGTextureReadOnlyID = m_RenderGrpah.ReadTexture2D(Name, Desc);
		FRGTexture2DID RGTextureID = RGTextureReadOnlyID.GetResourceID();
		m_RgPass.m_Texture2DReads.insert(RGTextureID);
		return RGTextureReadOnlyID;
	}

	FRGTexCubeReadOnlyID FRenderGraphBuilder::ReadTextureCubeImpl(FRGResourceName Name, ERGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTexCubeReadOnlyID RGTextureReadOnlyID = m_RenderGrpah.ReadTextureCube(Name, Desc);
		FRGTextureCubeID RGTextureID = RGTextureReadOnlyID.GetResourceID();
		m_RgPass.m_TextureCubeReads.insert(RGTextureID);
		return RGTextureReadOnlyID;
	}

	FRGTex2DReadWriteID FRenderGraphBuilder::WriteTexture2DImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTex2DReadWriteID RGTextureWriteOnlyID = m_RenderGrpah.WriteTexture2D(Name, Desc);
		FRGTexture2DID RGTextureID = RGTextureWriteOnlyID.GetResourceID();
		if (!m_RgPass.m_Texture2DCreates.contains(RGTextureID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture2D(Name);
		}
		m_RgPass.m_Texture2DWrites.insert(RGTextureID);
		FRGTexture2D* Texture = m_RenderGrpah.GetRGTexture2D(RGTextureID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGTextureWriteOnlyID;
	}

	FRGTexCubeReadWriteID FRenderGraphBuilder::WriteTextureCubeImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTexCubeReadWriteID RGTextureWriteOnlyID = m_RenderGrpah.WriteTextureCube(Name, Desc);
		FRGTextureCubeID RGTextureID = RGTextureWriteOnlyID.GetResourceID();
		if (!m_RgPass.m_TextureCubeCreates.contains(RGTextureID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTextureCube(Name);
		}
		m_RgPass.m_TextureCubeWrites.insert(RGTextureID);
		FRGTextureCube* Texture = m_RenderGrpah.GetRGTextureCube(RGTextureID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGTextureWriteOnlyID;
	}

	FRGTex2DRenderTargetID FRenderGraphBuilder::WriteRenderTarget2DImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTex2DRenderTargetID RenderTargetID = m_RenderGrpah.RenderTarget2D(Name, Desc);
		FRGTexture2DID ResID = RenderTargetID.GetResourceID();
		FRGPassBase::FRenderTargetInfo RenderTargetInfo
		{
			.RGRenderTarget2DID = RenderTargetID,
			.RenderTargetAccess = LoadStoreOp,
		};
		m_RgPass.m_RenderTergetInfo.push_back(RenderTargetInfo);
		if (!m_RgPass.m_Texture2DCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture2D(Name);
		}
		m_RgPass.m_Texture2DWrites.insert(m_RenderGrpah.GetTexture2DID(Name));
		FRGTexture2D* Texture = m_RenderGrpah.GetRGTexture2D(ResID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RenderTargetID;
	}

	FRGTex2DDepthStencilID FRenderGraphBuilder::WriteDepthStencil2DImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOp, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTex2DDepthStencilID DepthStencilID = m_RenderGrpah.DepthStencil2D(Name, Desc);
		FRGTexture2DID ResID = DepthStencilID.GetResourceID();
		FRGPassBase::FDepthStencilInfo DepthStencilInfo
		{
			.RGTex2DDepthStencilID = DepthStencilID,
			.DepthAccess = LoadStoreOp,
			.StencilAccess = StencilLoadStoreOp,
			.bReadOnly = false
		};
		m_RgPass.m_DepthStencil = DepthStencilInfo;
		if (!m_RgPass.m_Texture2DCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture2D(Name);
		}
		m_RgPass.m_Texture2DWrites.insert(m_RenderGrpah.GetTexture2DID(Name));
		FRGTexture2D* Texture = m_RenderGrpah.GetRGTexture2D(ResID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return DepthStencilID;
	}

	FRGTexCubeDepthStencilID FRenderGraphBuilder::WriteDepthStencilCubeImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOp, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTexCubeDepthStencilID DepthStencilID = m_RenderGrpah.DepthStencilCube(Name, Desc);
		FRGTextureCubeID ResID = DepthStencilID.GetResourceID();
		FRGPassBase::FDepthStencilInfo DepthStencilInfo
		{
			.RGTexCubeDepthStencilID = DepthStencilID,
			.DepthAccess = LoadStoreOp,
			.StencilAccess = StencilLoadStoreOp,
			.bReadOnly = false
		};
		m_RgPass.m_DepthStencil = DepthStencilInfo;
		if (!m_RgPass.m_TextureCubeCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTextureCube(Name);
		}
		m_RgPass.m_TextureCubeWrites.insert(m_RenderGrpah.GetTextureCubeID(Name));
		FRGTextureCube* Texture = m_RenderGrpah.GetRGTextureCube(ResID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return DepthStencilID;
	}

	FRGTex2DDepthStencilID FRenderGraphBuilder::ReadDepthStencil2DImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOps, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTex2DDepthStencilID RGDepthStencilID = m_RenderGrpah.DepthStencil2D(Name, Desc);
		FRGTexture2DID ResID = RGDepthStencilID.GetResourceID();
		FRGPassBase::FDepthStencilInfo DepthStencilInfo
		{
			.RGTex2DDepthStencilID = RGDepthStencilID,
			.DepthAccess = LoadStoreOp,
			.StencilAccess = StencilLoadStoreOps,
			.bReadOnly = false
		};
		FRGTexture2D* Texture = m_RenderGrpah.GetRGTexture2D(ResID);
		m_RgPass.m_Texture2DReads.insert(ResID);


		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGDepthStencilID;
	}

	FRGTexCubeDepthStencilID FRenderGraphBuilder::ReadDepthStencilCubeImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, ERGLoadStoreAccessOp StencilLoadStoreOps, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTexCubeDepthStencilID RGDepthStencilID = m_RenderGrpah.DepthStencilCube(Name, Desc);
		FRGTextureCubeID ResID = RGDepthStencilID.GetResourceID();
		FRGPassBase::FDepthStencilInfo DepthStencilInfo
		{
			.RGTexCubeDepthStencilID= RGDepthStencilID,
			.DepthAccess = LoadStoreOp,
			.StencilAccess = StencilLoadStoreOps,
			.bReadOnly = false
		};
		FRGTextureCube* Texture = m_RenderGrpah.GetRGTextureCube(ResID);
		m_RgPass.m_TextureCubeReads.insert(ResID);


		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGDepthStencilID;
	}	

	FRGTexCubeRenderTargetID FRenderGraphBuilder::WriteRenderTargetCubeImpl(FRGResourceName Name, ERGLoadStoreAccessOp LoadStoreOp, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERenderPassType::Copy, "Invalid Call in Copy Pass");
		FRGTexCubeRenderTargetID RenderTargetID = m_RenderGrpah.RenderTargetCube(Name, Desc);
		FRGTextureCubeID ResID = RenderTargetID.GetResourceID();
		FRGPassBase::FRenderTargetInfo RenderTargetInfo
		{
			.RGRenderTargetCubeID = RenderTargetID,
			.RenderTargetAccess = LoadStoreOp,
		};
		m_RgPass.m_RenderTergetInfo.push_back(RenderTargetInfo);
		if (!m_RgPass.m_TextureCubeCreates.contains(ResID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTextureCube(Name);
		}
		m_RgPass.m_TextureCubeWrites.insert(m_RenderGrpah.GetTextureCubeID(Name));
		FRGTextureCube* Texture = m_RenderGrpah.GetRGTextureCube(ResID);
		if (Texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RenderTargetID;
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

	void FRenderGraphBuilder::DummyWriteTexture2D(FRGResourceName Name)
	{
		m_RgPass.m_Texture2DWrites.insert(m_RenderGrpah.GetTexture2DID(Name));
	}

	void FRenderGraphBuilder::DummyReadTexture2D(FRGResourceName Name)
	{
		m_RgPass.m_Texture2DReads.insert(m_RenderGrpah.GetTexture2DID(Name));
	}

	void FRenderGraphBuilder::DummyWriteTextureCube(FRGResourceName Name)
	{
		m_RgPass.m_TextureCubeWrites.insert(m_RenderGrpah.GetTextureCubeID(Name));
	}

	void FRenderGraphBuilder::DummyReadTextureCube(FRGResourceName Name)
	{
		m_RgPass.m_TextureCubeReads.insert(m_RenderGrpah.GetTextureCubeID(Name));
	}

	void FRenderGraphBuilder::DummyReadBuffer(FRGResourceName Name)
	{
		m_RgPass.m_BufferReads.insert(m_RenderGrpah.GetBufferID(Name));
	}

	void FRenderGraphBuilder::DummyWriteBuffer(FRGResourceName Name)
	{
		m_RgPass.m_BufferWrites.insert(m_RenderGrpah.GetBufferID(Name));
	}

	FRGTexture2DCopySrcID FRenderGraphBuilder::ReadCopySrcTexture(FRGResourceName Name)
	{
		FRGTexture2DCopySrcID RGTextureCopySrcID = m_RenderGrpah.ReadCopySrcTexture2D(Name);
		FRGTexture2DID RGTextureID(RGTextureCopySrcID);
		m_RgPass.m_Texture2DReads.insert(RGTextureID);
		return RGTextureCopySrcID;
	}

	FRGTexture2DCopyDstID FRenderGraphBuilder::WriteCopyDstTexture(FRGResourceName Name)
	{
		FRGTexture2DCopyDstID RGTextureCopyDstID = m_RenderGrpah.WriteCopyDstTexture2D(Name);
		FRGTexture2DID RGTextureID(RGTextureCopyDstID);
		if (!m_RgPass.m_Texture2DCreates.contains(RGTextureID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
			DummyReadTexture2D(Name);
		}
		m_RgPass.m_Texture2DWrites.insert(RGTextureID);
		auto* texture = m_RenderGrpah.GetRGTexture2D(RGTextureID);
		if (texture->bImported)
		{
			m_RgPass.m_Flags |= ERGPassFlags::ForceNoCull;
		}
		return RGTextureCopyDstID;
	}

	bool FRenderGraphBuilder::IsTexture2DDeclared(FRGResourceName Name)
	{
		return m_RenderGrpah.IsTexture2DDeclared(Name);
	}

	bool FRenderGraphBuilder::IsTextureCubeDeclared(FRGResourceName Name)
	{
		return m_RenderGrpah.IsTextureCubeDeclared(Name);
	}

	bool FRenderGraphBuilder::IsBufferDeclared(FRGResourceName Name)
	{
		return m_RenderGrpah.IsBufferDeclared(Name);
	}

	void FRenderGraphBuilder::DeclareTexture2D(FRGResourceName Name, const FRGTextureDesc& Desc)
	{
		m_RgPass.m_Texture2DCreates.insert(m_RenderGrpah.DeclareTexture2D(Name, Desc));
	}

	void FRenderGraphBuilder::DeclareTextureCube(FRGResourceName Name, const FRGTextureDesc& desc)
	{
		m_RgPass.m_TextureCubeCreates.insert(m_RenderGrpah.DeclareTextureCube(Name, desc));
	}

	void FRenderGraphBuilder::DeclareBuffer(FRGResourceName Name, const FRGBufferDesc& Desc)
	{
		m_RgPass.m_BufferCreates.insert(m_RenderGrpah.DeclareBuffer(Name, Desc));
	}
}
