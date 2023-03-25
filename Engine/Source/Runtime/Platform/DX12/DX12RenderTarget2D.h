#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "DX12Texture2D.h"
#include "Render/RHI/RenderTarget.h"


namespace Zero
{
	class FDX12Texture2D;
	class FDX12Device;
	class FDX12RenderTarget2D : public FRenderTarget2D
	{
	public:
		FDX12RenderTarget2D();
		FDX12RenderTarget2D(FRenderTarget2DDesc Desc);
		virtual void ClearBuffer(FCommandListHandle CommandListHandle) override;
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t Depth);
		virtual void AttachColorTexture(uint32_t AttachmentIndex, FTexture2D* Texture2D, std::optional<FTextureClearValue> ClearValue = std::nullopt, uint32_t ViewID = 0) override;
		virtual void AttachDepthTexture(FTexture2D* Texture2D) override;
		virtual void Bind(FCommandListHandle CommanListHandle, bool bClearBuffer = true) override;
		virtual void UnBind(FCommandListHandle CommandListHandle);
		virtual void UnBindDepth(FCommandListHandle CommandListHandle) override;
		void SetViewportRect();
	private:

		// Get the render target formats of the textures currently
		// attached to this render target object.
		// This is needed to configure the Pipeline state object.
		D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;

		// Get the format of the attached Depth/Stencil buffer.
		DXGI_FORMAT GetDepthStencilFormat() const;

		// Get the sample description of the render target.
		DXGI_SAMPLE_DESC GetSampleDesc() const;

		D3D12_VIEWPORT m_ViewPort = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		D3D12_RECT m_ScissorRect = {0, 0, 0, 0};
	};
}