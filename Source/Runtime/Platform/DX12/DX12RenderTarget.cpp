#include "DX12RenderTarget.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include "Render/RendererAPI.h"
#include "Colors.h"

namespace Zero
{

	FDX12RenderTarget::FDX12RenderTarget()
		:FRenderTarget()
	{
	}

	FDX12RenderTarget::FDX12RenderTarget(FRenderTargetDesc Desc)
	{
		m_Width = Desc.Width;
		m_Height = Desc.Height;
		for (int Index = EAttachmentIndex::Color0; Index <= EAttachmentIndex::Color7; ++Index)
		{
			const ETextureFormat& TextureFormat = Desc.Format[Index];
			if (TextureFormat == ETextureFormat::None)
				continue;
			DXGI_FORMAT DxgiFormat = FDX12Texture2D::GetFormatByDesc(TextureFormat);

			Ref<FTexture2D> Texture;
			EAttachmentIndex AttachMentIndex = EAttachmentIndex(Index);
			D3D12_RESOURCE_DESC RtvResourceDesc;
			RtvResourceDesc.Alignment = 0;
			RtvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			RtvResourceDesc.DepthOrArraySize = 1;
			RtvResourceDesc.Width = m_Width;
			RtvResourceDesc.Height = m_Height;
			RtvResourceDesc.MipLevels = 1;
			RtvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			RtvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			RtvResourceDesc.Format = DxgiFormat;
			RtvResourceDesc.SampleDesc.Count = 1;
			RtvResourceDesc.SampleDesc.Quality = 0;
			CD3DX12_CLEAR_VALUE OptClear;
			OptClear.Format = DxgiFormat;
			memcpy(OptClear.Color, DirectX::Colors::Transparent, 4 * sizeof(float));
			Texture = CreateRef<FDX12Texture2D>(RtvResourceDesc, &OptClear);
			AttachTexture(AttachMentIndex, Texture);
#ifdef EDITOR_MODE
			Texture->RegistGuiShaderResource();
#endif
		}

		DXGI_FORMAT DepthDxgiFormat = FDX12Texture2D::GetFormatByDesc(Desc.Format[EAttachmentIndex::DepthStencil]);
		if (DepthDxgiFormat != DXGI_FORMAT_UNKNOWN)
		{
			auto DepthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, m_Width, m_Height);
			DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE OptClear = {};
			OptClear.Format = DepthDxgiFormat;
			OptClear.DepthStencil = { 1.0F, 0 };
			Ref<FDX12Texture2D> Texture = CreateRef<FDX12Texture2D>(DepthStencilDesc, &OptClear);
			AttachTexture(EAttachmentIndex::DepthStencil, Texture);
		}
	}

	void FDX12RenderTarget::ClearBuffer()
	{
		auto CommandList = FDX12Device::Get()->GetRenderCommandList();
		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_Textures[i].get());
			if (Texture != nullptr)
			{
				CommandList->ClearTexture(Texture, Utils::Colors::Transparent);
			}
		}
		auto* DepthStencilTexture = static_cast<FDX12Texture2D*>(m_Textures[EAttachmentIndex::DepthStencil].get());
		CommandList->ClearDepthStencilTexture(DepthStencilTexture, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
	}

	void FDX12RenderTarget::Resize(uint32_t Width, uint32_t Height, uint32_t Depth)
	{
		m_Width = Width;
		m_Height = Height;

		for (int i = 0; i < EAttachmentIndex::NumAttachmentPoints; ++i)
		{
			if (m_Textures[i] != nullptr)
			{
				m_Textures[i]->Resize(Width, Height, Depth);
#ifdef EDITOR_MODE
				if (i != EAttachmentIndex::DepthStencil)
				{
					m_Textures[i]->RegistGuiShaderResource();
				}
#endif //  EDIT_MODE

			}
		}

		SetViewportRect();
	}

	void FDX12RenderTarget::AttachTexture(EAttachmentIndex AttachmentIndex, Ref<FTexture2D> Texture2D)
	{
		size_t Index = size_t(AttachmentIndex);
		m_Textures[Index] = Texture2D;

		if (m_Textures[Index].get() != nullptr)
		{
			ZMath::uvec2 Size = m_Textures[Index]->GetSize();
			m_Width = Size.x;
			m_Height = Size.y;
			SetViewportRect();
		}
	}

	void FDX12RenderTarget::Bind()
	{
		auto  CommandList = FDX12Device::Get()->GetRenderCommandList();
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> Handles;
		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_Textures[i].get());
			if (Texture)
			{
				CommandList->TransitionBarrier(Texture->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
				Handles.push_back(Texture->GetRenderTargetView());
			}
		}
		CommandList->GetD3D12CommandList()->RSSetViewports(1, &m_ViewPort);
		CommandList->GetD3D12CommandList()->RSSetScissorRects(1, &m_ScissorRect);
		if (m_Textures[EAttachmentIndex::DepthStencil].get() != nullptr)
		{
			auto* DsvTexture = static_cast<FDX12Texture2D*>(m_Textures[EAttachmentIndex::DepthStencil].get());
			CommandList->GetD3D12CommandList()->OMSetRenderTargets(
				UINT(Handles.size()),
				Handles.data(),
				false,
				&DsvTexture->GetDepthStencilView()
			);
		}
		else
		{
			CommandList->GetD3D12CommandList()->OMSetRenderTargets(
				UINT(Handles.size()),
				Handles.data(),
				false,
				nullptr
			);
		}
	}

	void FDX12RenderTarget::UnBind()
	{
		FDX12Device::Get()->GetSwapChain()->SetRenderTarget();

		auto  CommandList = FDX12Device::Get()->GetRenderCommandList();
		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_Textures[i].get());
			if (Texture)
			{
				CommandList->TransitionBarrier(Texture->GetD3DResource(), D3D12_RESOURCE_STATE_COMMON);
			}
		}
	}

	void FDX12RenderTarget::SetViewportRect()
	{
		// Set Viewport
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = (float)m_Width;
		m_ViewPort.Height = (float)m_Height;
		m_ViewPort.MaxDepth = 1.0f;
		m_ViewPort.MinDepth = 0.0f;

		// Set Scissor Rect
		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = m_Width;
		m_ScissorRect.bottom = m_Height;
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
