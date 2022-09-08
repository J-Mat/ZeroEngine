#include "DX12RenderTarget.h"
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{

	FDX12RenderTarget::FDX12RenderTarget(FDX12Device& Device)
		: m_Device(Device)
		, FRenderTarget()
	{
	}

	void FDX12RenderTarget::ClearBuffer()
	{
		auto CommandList = m_Device.GetRenderCommandList();
		for (int i = EAttachmentIndex::Color0; i <= EAttachmentIndex::Color7; ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_Textures[i].get());
			if (Texture != nullptr)
			{
				CommandList->ClearTexture(Texture, ZMath::vec4(1.0f, 0.0f, 0.0f, 1.0f));
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
			m_Textures[i]->Resize(Width, Height, Depth);
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
		auto  CommandList = m_Device.GetRenderCommandList();
		CommandList->GetD3D12CommandList()->RSSetViewports(1, &m_ViewPort);
		CommandList->GetD3D12CommandList()->RSSetScissorRects(1, &m_ScissorRect);
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
		if (m_Textures[EAttachmentIndex::DepthStencil].get() != nullptr)
		{
			auto* DsvTexture = static_cast<FDX12Texture2D*>(m_Textures[EAttachmentIndex::DepthStencil].get());
			CommandList->GetD3D12CommandList()->OMSetRenderTargets(
				UINT(Handles.size()),
				Handles.data(),
				true,
				&DsvTexture->GetDepthStencilView()
			);
		}
		else
		{
			CommandList->GetD3D12CommandList()->OMSetRenderTargets(
				UINT(Handles.size()),
				Handles.data(),
				true,
				nullptr
			);
		}
	}

	void FDX12RenderTarget::UnBind()
	{
		auto  CommandList = m_Device.GetRenderCommandList();
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
