#include "RenderGraphBuilder.h"
#include "RenderGraph.h"

namespace Zero
{

	FRGTextureReadOnlyID FRenderGraphBuilder::ReadTextureImpl(FRGResourceName Name, RGReadAccess ReadAcess, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERGPassType::Copy, "Invalid Call in Copy Pass");
		FRGTextureReadOnlyID RGTextureReadOnlyID = m_RenderGrpah.ReadTexture(Name, Desc);
		FRGTextureID RGTextureID = RGTextureReadOnlyID.GetResourceID();
		if (m_RgPass.GetPassType() == ERGPassType::Graphics)
		{ 
			switch (ReadAcess)
			{
			case RGReadAccess::ReadAccess_PixelShader:
				m_RgPass.m_TextureStateMap[RGTextureID] = EResourceState::PixelShaderResource;
				break;
			case RGReadAccess::ReadAccess_NonPixelShader:
				m_RgPass.m_TextureStateMap[RGTextureID] = EResourceState::NonPixelShaderResource;
				break;
			case RGReadAccess::ReadAccess_AllShader:
				m_RgPass.m_TextureStateMap[RGTextureID] = EResourceState::AllShaderResource;
				break;
			default:
				CORE_ASSERT(false, "Invalid Read Flag!");
			}
		}
		else if (m_RgPass.GetPassType() == ERGPassType::Compute || m_RgPass.GetPassType() == ERGPassType::ComputeAsync)
		{
			m_RgPass.m_TextureStateMap[RGTextureID] = EResourceState::NonPixelShaderResource;
		}
		m_RgPass.m_TextureReads.insert(RGTextureID);
		return RGTextureReadOnlyID;
	}

	FRGTextureReadWriteID FRenderGraphBuilder::WriteTextureImpl(FRGResourceName Name, const FTextureSubresourceDesc& Desc)
	{
		CORE_ASSERT(m_RgPass.m_Type != ERGPassType::Copy, "Invalid Call in Copy Pass");
		FRGTextureReadWriteID RGTextureWriteOnlyID = m_RenderGrpah.WriteTexture(Name, Desc);
		FRGTextureID RGTextureID = RGTextureWriteOnlyID.GetResourceID();
		m_RgPass.m_TextureStateMap[RGTextureID] = EResourceState::UnorderedAccess;
		if (!m_RgPass.m_TextureCreates.contains(RGTextureID) && !m_RgPass.ActAsCreatorWhenWriting())
		{
		}
	}
	void FRenderGraphBuilder::DummyWriteTexture(FRGResourceName Name)
	{
	}

	void FRenderGraphBuilder::DummyReadTexture(FRGResourceName Name)
	{
	}

	void FRenderGraphBuilder::DummyReadBuffer(FRGResourceName Name)
	{
	}

	void FRenderGraphBuilder::DummyWriteBuffer(FRGResourceName Name)
	{
	}
}
