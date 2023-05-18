#include "RenderPass.h"
#include "../RenderGraph/RenderGraphResourcePool.h"
#include "../RenderGraph/RenderGraph.h"

namespace Zero
{
	FRenderPass::FRenderPass(FRenderGraph& Rg, FRenderPassDesc const& Desc):
		m_RenderGrpah(Rg),
		m_Width(Desc.Width),
		m_Height(Desc.Height),
		m_Depth(Desc.Depth),
		m_RenderPassRTType(Desc.RenderPassRTType)
	{
		CORE_ASSERT(m_RenderPassRTType != ERenderPassRTType::RenderPass_None, "m_RenderPassRTType must have value");
		if (m_RenderPassRTType == ERenderPassRTType::Texuture2D)
		{
			AttachRenderTarget2D(Desc);
		}
		else
		{
			AttachRenderTargetCube(Desc);
		}
	}

	void FRenderPass::AttachRenderTarget2D(FRenderPassDesc const& Desc)
	{
		m_RenderTarget2D = m_RenderGrpah.GetResourcePool().AllocateRenderTarget2D();
		m_RenderTarget2D->Resize(m_Width, m_Height);
		for (uint32_t i = 0; i < Desc.RtvAttachments.size(); ++i)
		{
			const FRtvAttachmentDesc& RtvAttachmentDesc = Desc.RtvAttachments[i];

			FRenderTex2DAttachment Attachment =
			{
				.Texture = RtvAttachmentDesc.RTTexture2D.value(),
				.ViewID = 0,
				.ClearValue = RtvAttachmentDesc.ClearValue,
				.bClearColor = RtvAttachmentDesc.BeginningAccess == ERGLoadAccessOp::Clear ? true : false,
			};
			m_RenderTarget2D->AttachColorTexture(i, Attachment);
		}
		if (Desc.DsvAttachment)
		{
			auto& DsvAttachmentDesc = Desc.DsvAttachment.value();
			FRenderTex2DAttachment Attachment =
			{
				.Texture = DsvAttachmentDesc.DSTexture2D.value(),
				.ViewID = 0,
				.ClearValue = DsvAttachmentDesc.ClearValue,
				.bClearColor = DsvAttachmentDesc.DepthBeginningAccess == ERGLoadAccessOp::Clear ? true : false,
			};
			m_RenderTarget2D->AttachDepthTexture(Attachment);
		}
	}

	void FRenderPass::AttachRenderTargetCube(FRenderPassDesc const& Desc)
	{
		m_RenderTargetCube = m_RenderGrpah.GetResourcePool().AllocateRenderTargetCube();
		m_RenderTargetCube->Resize(m_Width, m_Height);
		CORE_ASSERT(Desc.RtvAttachments.size() <= 1, "Desc.RtvAttachments.size() == 1");
		for (uint32_t i = 0; i < Desc.RtvAttachments.size(); ++i)
		{
			const FRtvAttachmentDesc& RtvAttachmentDesc = Desc.RtvAttachments[i];

			FRenderTexCubeAttachment Attachment =
			{
				.Texture = RtvAttachmentDesc.RTTextureCube.value(),
				.ViewID = 0,
				.ClearValue = RtvAttachmentDesc.ClearValue,
				.bClearColor = RtvAttachmentDesc.BeginningAccess == ERGLoadAccessOp::Clear ? true : false,
			};
			m_RenderTargetCube->AttachColorTexture(Attachment);
		}
		if (Desc.DsvAttachment)
		{
			auto& DsvAttachmentDesc = Desc.DsvAttachment.value();
			FRenderTexCubeAttachment Attachment =
			{
				.Texture = DsvAttachmentDesc.DSTextureCube.value(),
				.ViewID = 0,
				.ClearValue = DsvAttachmentDesc.ClearValue,
				.bClearColor = DsvAttachmentDesc.DepthBeginningAccess == ERGLoadAccessOp::Clear ? true : false,
			};
			m_RenderTargetCube->AttachDepthTexture(Attachment);
		}
	}

	void FRenderPass::Begin(FCommandListHandle CommandListHandle)
	{
		if (m_RenderPassRTType == ERenderPassRTType::Texuture2D)
		{
			m_RenderTarget2D->Bind(CommandListHandle);
		}
		else
		{
			m_RenderTargetCube->Bind(CommandListHandle);
			m_RenderTargetCube->SetRenderTarget(CommandListHandle, m_Depth);
		}
		
	}

	void FRenderPass::End(FCommandListHandle CommandListHandle)
	{
		if (m_RenderPassRTType == ERenderPassRTType::Texuture2D)
		{
			m_RenderTarget2D->UnBind(CommandListHandle);
			m_RenderGrpah.GetResourcePool().ReleaseRenderTarget2D(m_RenderTarget2D);
		}
		else
		{
			if (m_Depth == 5)
			{
				m_RenderTargetCube->UnBind(CommandListHandle, 0);
			}
			m_RenderGrpah.GetResourcePool().ReleaseRenderTargetCube(m_RenderTargetCube);
		}
	}
}

