#pragma once
#include "Core.h"
#include "Render/RHI/ResourceView.h"
#include "../RHI/Texture.h"
#include "../RHI/RenderTarget.h"

namespace Zero
{
	enum EATTACHMENT_LOAD_OP : uint8_t
	{
		/// The previous contents of the texture within the render area will be preserved.
		/// Vulkan counterpart: VK_ATTACHMENT_LOAD_OP_LOAD.
		/// D3D12 counterpart: D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE.
		ATTACHMENT_LOAD_OP_LOAD = 0,

		/// The contents within the render area will be cleared to a uniform value, which is
		/// specified when a render pass instance is begun.
		/// Vulkan counterpart: VK_ATTACHMENT_LOAD_OP_CLEAR.
		/// D3D12 counterpart: D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR.
		ATTACHMENT_LOAD_OP_CLEAR,

		/// The previous contents within the area need not be preserved; the contents of
		/// the attachment will be undefined inside the render area.
		/// Vulkan counterpart: VK_ATTACHMENT_LOAD_OP_DONT_CARE.
		/// D3D12 counterpart: D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD.
		ATTACHMENT_LOAD_OP_DISCARD,

		ATTACHMENT_LOAD_OP_COUNT
	};


	/// Render pass attachment store operation
	/// Vulkan counterpart: [VkAttachmentStoreOp](https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VkAttachmentStoreOp).
	/// D3D12 counterpart: [D3D12_RENDER_PASS_ENDING_ACCESS_TYPE](https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_render_pass_ending_access_type).
	enum EATTACHMENT_STORE_OP : uint8_t
	{
		/// The contents generated during the render pass and within the render area are written to memory.
		/// Vulkan counterpart: VK_ATTACHMENT_STORE_OP_STORE.
		/// D3D12 counterpart: D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE.
		ATTACHMENT_STORE_OP_STORE = 0,

		/// The contents within the render area are not needed after rendering, and may be discarded;
		/// the contents of the attachment will be undefined inside the render area.
		/// Vulkan counterpart: VK_ATTACHMENT_STORE_OP_DONT_CARE.
		/// D3D12 counterpart: D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD.
		ATTACHMENT_STORE_OP_DISCARD,

		ATTACHMENT_STORE_OP_COUNT
	};


	struct FRtvAttachmentDesc
	{
		FTexture2D* RTTexture;
		EATTACHMENT_LOAD_OP BeginningAccess;
		EATTACHMENT_STORE_OP EndingAccess;
		FTextureClearValue ClearValue;
	};

	struct FDsvAttachmentDesc
	{
		FTexture2D* DSTexture;
		EATTACHMENT_LOAD_OP  DepthBeginningAccess;
		EATTACHMENT_STORE_OP    DepthEndingAccess;
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
