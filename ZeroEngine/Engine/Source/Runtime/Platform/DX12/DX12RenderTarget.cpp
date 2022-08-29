#include "DX12RenderTarget.h"

namespace Zero
{
	FDX12RenderTarget::FDX12RenderTarget()
		: FRenderTarget()
	{
	}

	void FDX12RenderTarget::Resize(uint32_t Width, uint32_t Height, uint32_t Depth)
	{
		for (int i = 0; i < EAttachmentIndex::NumAttachmentPoints; ++i)
		{
			m_Textures[i]->Resize(Width, Height, Depth);
		}
	}

	void FDX12RenderTarget::AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D)
	{
		size_t Index = size_t(AttachmentIndex);
		m_Textures[Index] = Texture2D;

		if (m_Textures[Index].get())
		{
			ZMath::uvec2 Size = m_Textures[Index]->GetSize();
			m_Width = Size.x;
			m_Height = Size.y;
		}
	}

	D3D12_RT_FORMAT_ARRAY FDX12RenderTarget::GetRenderTargetFormats() const
	{
		D3D12_RT_FORMAT_ARRAY RtvFormats = {};

		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_Textures[i].get());
			if (Texture)
			{
				RtvFormats.RTFormats[RtvFormats.NumRenderTargets++] = Texture->GetD3D12ResourceDesc().Format;
			}
		}

		return RtvFormats;
	}

	DXGI_FORMAT FDX12RenderTarget::GetDepthStencilFormat() const
	{
		DXGI_FORMAT DsvFormat = DXGI_FORMAT_UNKNOWN;
		auto DepthStencilTexture = static_cast<FDX12Texture2D*>(m_Textures[EAttachmentIndex::DepthStencil].get());
		if (DepthStencilTexture)
		{
			DsvFormat = DepthStencilTexture->GetD3D12ResourceDesc().Format;
		}
		return DsvFormat;
	}
	DXGI_SAMPLE_DESC FDX12RenderTarget::GetSampleDesc() const
	{
		DXGI_SAMPLE_DESC SampleDesc = { 1, 0 };
		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_Textures[i].get());
			if (Texture)
			{
				SampleDesc = Texture->GetD3D12ResourceDesc().SampleDesc;
				break;
			}
		}

		return SampleDesc;
	}
}
