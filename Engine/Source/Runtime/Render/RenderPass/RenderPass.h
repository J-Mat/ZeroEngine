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
		std::optional<FTexture2D*> RTTexture2D;
		std::optional<FTextureCube*> RTTextureCube;
		ERGLoadAccessOp BeginningAccess;
		ERGStoreAccessOp EndingAccess;
		FTextureClearValue ClearValue;
	};

	struct FDsvAttachmentDesc
	{
		std::optional<FTexture2D*> DSTexture2D;
		std::optional<FTextureCube*> DSTextureCube;
		ERGLoadAccessOp  DepthBeginningAccess;
		ERGStoreAccessOp    DepthEndingAccess;
		//EATTACHMENT_LOAD_OP    StencilBeginningAccess = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
		//EATTACHMENT_STORE_OP    StencilEndingAccess = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
		FTextureClearValue ClearValue;
	};


	struct FRenderPassDesc
	{
		ERenderPassRTType RenderPassRTType = ERenderPassRTType::RenderPass_None;
		std::vector<FRtvAttachmentDesc> RtvAttachments{};
		std::optional<FDsvAttachmentDesc> DsvAttachment = std::nullopt;
		uint32_t Width;
		uint32_t Height;
		uint32_t Depth = 0;
	};

	class FRenderGraph;
	class FRenderPass
	{
	public:
		FRenderPass() = default;
		explicit FRenderPass(FRenderGraph& Rg, FRenderPassDesc const& Desc);

		void AttachRenderTarget2D(FRenderPassDesc const& Desc);
		void AttachRenderTargetCube(FRenderPassDesc const& Desc);
		void Begin(FCommandListHandle CommandListHandle);
		void End(FCommandListHandle CommandListHandle);
	private:	
		FRenderTarget2D* m_RenderTarget2D = nullptr;
		FRenderTargetCube* m_RenderTargetCube = nullptr;
		FRenderGraph &m_RenderGrpah; 
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Depth;
		ERenderPassRTType m_RenderPassRTType = ERenderPassRTType::RenderPass_None;
	};
}
