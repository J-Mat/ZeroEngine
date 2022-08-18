#include "DX12RenderTarget.h"

namespace Zero
{
	FDX12RenderTarget::FDX12RenderTarget()
		: FRenderTarget()
	{
	}

	void FDX12RenderTarget::Resize(uint32_t Width, uint32_t Height)
	{
		for (int i = 0; i < EAttachmentIndex::NumAttachmentPoints; ++i)
		{
			m_Textures[i]->Resize(Width, Height);
		}
	}

	void FDX12RenderTarget::AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D)
	{
		//FDX12Texture2D* test = dynamic_cast<FDX12Texture2D*>(Texture2D.get());
		m_Textures[AttachmentIndex] = Texture2D;

		if (m_Textures[AttachmentIndex] && m_Textures[AttachmentIndex].get())
		{
			auto Desc = m_Textures[AttachmentIndex]->GetD3D12ResourceDesc();
			m_Width = static_cast<uint32_t>(Desc.Width);
			m_Height = static_cast<uint32_t>(Desc.Height);
		}
	}

	void FDX12RenderTarget::Reset()
	{
	}

	D3D12_RT_FORMAT_ARRAY FDX12RenderTarget::GetRenderTargetFormats() const
	{
		D3D12_RT_FORMAT_ARRAY RtvFormats = {};

		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto Texture = m_Textures[i];
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
		auto DepthStencilTexture = m_Textures[EAttachmentIndex::DepthStencil];
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
			auto Texture = m_Textures[i];
			if (Texture)
			{
				SampleDesc = Texture->GetD3D12ResourceDesc().SampleDesc;
				break;
			}
		}

		return SampleDesc;
	}
}
