#include "RenderPass.h"
#include "../RenderGraph/RenderGraphResourcePool.h"
#include "../RenderGraph/RenderGraph.h"

namespace Zero
{
	FRenderPass::FRenderPass(FRenderGraph& Rg, FRenderPassDesc const& Desc):
		m_RenderGrpah(Rg),
		m_Width(Desc.Width),
		m_Height(Desc.Height)
	{
		m_RenderTarget = m_RenderGrpah.GetResourcePool().AllocateRenderTarget();
		for (uint32_t i = 0; i < Desc.RtvAttachments.size(); ++i)
		{
			const FRtvAttachmentDesc& RtvAttachmentDesc = Desc.RtvAttachments[i];

			FRenderTexAttachment Attachment =
			{
				.Texture = RtvAttachmentDesc.RTTexture,
				.ViewID = 0,
				.ClearValue = RtvAttachmentDesc.ClearValue,
				.bClearColor = RtvAttachmentDesc.BeginningAccess == ERGLoadAccessOp::Clear ? true : false,
			};
			m_RenderTarget->AttachColorTexture(i, Attachment);
		}
		if (Desc.DsvAttachment)
		{
			auto& DsvAttachmentDesc = Desc.DsvAttachment.value();
			FRenderTexAttachment Attachment =
			{
				.Texture = DsvAttachmentDesc.DSTexture,
				.ViewID = 0,
				.ClearValue = DsvAttachmentDesc.ClearValue,
				.bClearColor = DsvAttachmentDesc.DepthBeginningAccess == ERGLoadAccessOp::Clear ? true : false,
			};
			m_RenderTarget->AttachDepthTexture(Attachment);
		}
	}

	void FRenderPass::Begin(FCommandListHandle CommandListHandle)
	{
		m_RenderTarget->Bind(CommandListHandle);
	}

	void FRenderPass::End(FCommandListHandle CommandListHandle)
	{
		m_RenderTarget->UnBind(CommandListHandle);
		m_RenderGrpah.GetResourcePool().ReleaseRenderTarget(m_RenderTarget);
	}
}

