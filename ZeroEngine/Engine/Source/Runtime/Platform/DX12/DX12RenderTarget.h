#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "DX12Texture2D.h"
#include "Render/RHI/RenderTarget.h"


namespace Zero
{
	class FDX12Texture2D;
	class FDX12Device;
	class FDX12RenderTarget : public FRenderTarget
	{
	public:
		FDX12RenderTarget(FDX12Device& Device);
		virtual void Resize(uint32_t Width, uint32_t Height, uint32_t depth);
		virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D);
		virtual void Bind();
		virtual void UnBind();
		void SetViewportRect();
	private:
		FDX12Device& m_Device;
		// Get the render target formats of the textures currently
		// attached to this render target object.
		// This is needed to configure the Pipeline state object.
		D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;

		// Get the format of the attached depth/stencil buffer.
		DXGI_FORMAT GetDepthStencilFormat() const;

		// Get the sample description of the render target.
		DXGI_SAMPLE_DESC GetSampleDesc() const;

		D3D12_VIEWPORT m_ViewPort;
		D3D12_RECT m_ScissorRect;
	};
}