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
		virtual void ClearBuffer() override;
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t depth);
		virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D);
		virtual void Bind(bool bClearBuffer = true) override;
		virtual void UnBind();
		virtual void UnBindDepth() override;
		void SetViewportRect();
	private:

		// Get the render target formats of the textures currently
		// attached to this render target object.
		// This is needed to configure the Pipeline state object.
		D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;

		// Get the format of the attached depth/stencil buffer.
		DXGI_FORMAT GetDepthStencilFormat() const;

		// Get the sample description of the render target.
		DXGI_SAMPLE_DESC GetSampleDesc() const;

		D3D12_VIEWPORT m_ViewPort = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		D3D12_RECT m_ScissorRect = {0, 0, 0, 0};
	};
}