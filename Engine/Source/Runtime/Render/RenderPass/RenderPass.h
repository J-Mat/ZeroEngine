#pragma once
#include "Core.h"
#include "Render/RHI/ResourceView.h"
#include "../RHI/Texture.h"
#include "../RHI/RenderTarget.h"
#include "../RenderGraph/RenderGraphPass.h"

namespace Zero
{
	struct FRtvAttachmentDesc
	{
		FTexture2D* RTTexture;
		ERGLoadAccessOp BeginningAccess;
		ERGStoreAccessOp EndingAccess;
		FTextureClearValue ClearValue;
	};

	struct FDsvAttachmentDesc
	{
		FTexture2D* DSTexture;
		ERGLoadAccessOp  DepthBeginningAccess;
		ERGStoreAccessOp    DepthEndingAccess;
		//EATTACHMENT_LOAD_OP    StencilBeginningAccess = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
		//EATTACHMENT_STORE_OP    StencilEndingAccess = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
		FTextureClearValue ClearValue;
	};


	struct FRenderPassDesc
	{
		std::vector<FRtvAttachmentDesc> RtvAttachments{};
		std::optional<FDsvAttachmentDesc> DsvAttachment = std::nullopt;
		uint32_t Width;
		uint32_t Height;
	};

	class FRenderGraph;
	class FRenderPass
	{
	public:
		FRenderPass() = default;
		explicit FRenderPass(FRenderGraph& Rg, FRenderPassDesc const& Desc);

		void Begin(FCommandListHandle CommandListHandle);
		void End(FCommandListHandle CommandListHandle);
	private:	
		FRenderTarget2D* m_RenderTarget;
		FRenderGraph &m_RenderGrpah; 
		uint32_t m_Width;
		uint32_t m_Height;
	};
}
