#pragma once
#include "Core.h"
#include "Common/DX12Header.h"
#include "DX12Texture2D.h"
#include "Render/RHI/RenderTarget.h"


namespace Zero
{
	class FDX12RenderTarget : public FRenderTarget
	{
	public:
		FDX12RenderTarget();
		virtual Ref<FTexture2D> GetTexture(EAttachmentIndex AttachmentIndex) { return m_Textures[AttachmentIndex]; }
		virtual void Resize(uint32_t Width, uint32_t Height);



		virtual void AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D);


		virtual void Reset();
	private:
		using RenderTargetList = std::vector<Ref<FDX12Texture2D>>;
		RenderTargetList m_Textures;

		// Get the render target formats of the textures currently
		// attached to this render target object.
		// This is needed to configure the Pipeline state object.
		D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;

		// Get the format of the attached depth/stencil buffer.
		DXGI_FORMAT GetDepthStencilFormat() const;

		// Get the sample description of the render target.
		DXGI_SAMPLE_DESC GetSampleDesc() const;
	};
}