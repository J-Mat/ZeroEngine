#include "DX12RenderTarget2D.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include "Render/RendererAPI.h"
#include "Colors.h"

namespace Zero
{

	FDX12RenderTarget2D::FDX12RenderTarget2D()
		:FRenderTarget2D()
	{
	}

	FDX12RenderTarget2D::FDX12RenderTarget2D(FRenderTarget2DDesc RTDesc)
		:FRenderTarget2D()
	{
		m_Width = RTDesc.Width;
		m_Height = RTDesc.Height;
		for (int32_t Index = 0; Index < RTDesc.ColorFormat.size(); ++Index)
		{
			/*
			const EResourceFormat& TextureFormat = RTDesc.ColorFormat[Index];
			Ref<FTexture2D> Texture;
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
			*/

			const EResourceFormat& TextureFormat = RTDesc.ColorFormat[Index];
			std::string TextureName = std::format("{0}_Color_{1}", RTDesc.RenderTargetName, Index);
			FTextureClearValue ClearValue{};
			FTextureDesc Desc = {
				.Width = m_Width,
				.Height = m_Height,
				.ResourceBindFlags = (EResourceBindFlag::RenderTarget | EResourceBindFlag::ShaderResource),
				.InitialState = EResourceState::Common,
				.Format = RTDesc.ColorFormat[Index],
			};
			FTexture2D* Texture = new FDX12Texture2D(TextureName, Desc, &ClearValue);
			AttachColorTexture(Index, Texture);
#ifdef EDITOR_MODE
				Texture->RegistGuiShaderResource();
#endif
		}

		if (RTDesc.bNeedDepth)
		{
			//.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS
			std::string TextureName = std::format("{0}_Depth", RTDesc.RenderTargetName);
			FDX12Texture2D* DepthTexture = FDX12Device::Get()->CreateDepthTexture(TextureName, m_Width, m_Height);
			AttachDepthTexture(DepthTexture);
		}
		SetViewportRect();
	}

	void FDX12RenderTarget2D::ClearBuffer(FCommandListHandle CommandListHandle)
	{
		auto CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		for (int i = 0; i < m_ColoTexture.size(); ++i)
		{ auto* Texture = static_cast<FDX12Texture2D*>(m_ColoTexture[i].Texture);
			if (Texture != nullptr)
			{
				if (m_ColoTexture[i].ClearValue)
				{
					auto ClearValue = m_ColoTexture[i].ClearValue.value();
					CommandList->ClearTexture(Texture, ClearValue.Color.ToVec4());
				}
				CommandList->ClearTexture(Texture, Utils::Colors::Transparent);
			}
		}
		auto* DepthStencilTexture = static_cast<FDX12Texture2D*>(m_DepthTexture);
		if (DepthStencilTexture != nullptr)
		{
			CommandList->ClearDepthStencilTexture(DepthStencilTexture, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
		}
	}

	void FDX12RenderTarget2D::Resize(uint32_t Width, uint32_t Height, uint32_t Depth)
	{
		m_Width = Width;
		m_Height = Height;

		for (size_t i = 0; i < m_ColoTexture.size(); ++i)
		{
			if (m_ColoTexture[i].Texture != nullptr)
			{
				m_ColoTexture[i].Texture->Resize(Width, Height, Depth);
				m_ColoTexture[i].Texture->RegistGuiShaderResource();
			}
		}

		if (m_DepthTexture != nullptr)
		{
			m_DepthTexture->Resize(Width, Height, Depth);
		}
		
		SetViewportRect();
	}

	void FDX12RenderTarget2D::AttachColorTexture(uint32_t AttachmentIndex, FTexture2D* Texture2D, std::optional<FTextureClearValue> ClearValue /*= std::nullopt*/, uint32_t ViewID /*= 0*/)
	{
		m_ColoTexture[AttachmentIndex] = { Texture2D, ViewID, ClearValue };

		if (m_ColoTexture[AttachmentIndex].Texture != nullptr)
		{
			ZMath::uvec2 Size = m_ColoTexture[AttachmentIndex].Texture->GetSize();
			m_Width = Size.x;
			m_Height = Size.y;
			SetViewportRect();
		}
	}

	void FDX12RenderTarget2D::AttachDepthTexture(FTexture2D* Texture2D)
	{
		m_DepthTexture = Texture2D;
	}

	void FDX12RenderTarget2D::Bind(FCommandListHandle CommandListHandle, bool bClearBuffer)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		CommandList->GetD3D12CommandList()->RSSetViewports(1, &m_ViewPort);
		CommandList->GetD3D12CommandList()->RSSetScissorRects(1, &m_ScissorRect);
		if (bClearBuffer)
		{
			ClearBuffer(CommandListHandle);
		}
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> Handles;
		for (int i = 0; i < m_ColoTexture.size(); ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_ColoTexture[i].Texture);
			if (Texture)
			{
				auto Resource = Texture->GetResource();
				CommandList->TransitionBarrier(Resource->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
				FDX12RenderTargetView* Rtv = static_cast<FDX12RenderTargetView*>(Texture->GetRTV());
				Handles.push_back(Rtv->GetDescriptorHandle());
			}
		}
		if (m_DepthTexture != nullptr)
		{
			auto* DsvTexture = static_cast<FDX12Texture2D*>(m_DepthTexture);
			if (Handles.size() == 0)
			{ 
				FDX12DepthStencilView* Dsv = static_cast<FDX12DepthStencilView*>(DsvTexture->GetDSV());
				CommandList->GetD3D12CommandList()->OMSetRenderTargets(0, nullptr, false, &Dsv->GetDescriptorHandle());
			}
			else
			{
				FDX12DepthStencilView* Dsv = static_cast<FDX12DepthStencilView*>(DsvTexture->GetDSV());
				CommandList->GetD3D12CommandList()->OMSetRenderTargets(
					UINT(Handles.size()),
					Handles.data(),
					false,
					&Dsv->GetDescriptorHandle()
				);
			}
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

	void FDX12RenderTarget2D::UnBind(FCommandListHandle CommandListHandle)
	{
		FDX12Device::Get()->GetSwapChain()->SetRenderTarget(CommandListHandle);

		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		for (int i = 0; i < m_ColoTexture.size(); ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_ColoTexture[i].Texture);
			if (Texture)
			{
				auto Resource = Texture->GetResource();
				CommandList->TransitionBarrier(Resource->GetD3DResource(), D3D12_RESOURCE_STATE_COMMON);
			}
		}
		UnBindDepth(CommandListHandle);
	}

	void FDX12RenderTarget2D::UnBindDepth(FCommandListHandle CommandListHandle)
	{
		auto* DepthStencilTexture = static_cast<FDX12Texture2D*>(m_DepthTexture);
		if (DepthStencilTexture != nullptr)
		{
			auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
			auto Resource = DepthStencilTexture->GetResource();
			CommandList->TransitionBarrier(Resource->GetD3DResource(), D3D12_RESOURCE_STATE_COMMON);
		}
	}

	void FDX12RenderTarget2D::SetViewportRect()
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

	D3D12_RT_FORMAT_ARRAY FDX12RenderTarget2D::GetRenderTargetFormats() const
	{
		D3D12_RT_FORMAT_ARRAY RtvFormats = {};

		for (int i = 0; i <= m_ColoTexture.size(); ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_ColoTexture[i].Texture);
			if (Texture)
			{
				auto Resource = Texture->GetResource();
				RtvFormats.RTFormats[RtvFormats.NumRenderTargets++] = Resource->GetD3D12ResourceDesc().Format;
			}
		}

		return RtvFormats;
	}

	DXGI_FORMAT FDX12RenderTarget2D::GetDepthStencilFormat() const
	{
		DXGI_FORMAT DsvFormat = DXGI_FORMAT_UNKNOWN;
		auto DepthStencilTexture = static_cast<FDX12Texture2D*>(m_DepthTexture);
		if (DepthStencilTexture)
		{
			auto Resource = DepthStencilTexture->GetResource();
			DsvFormat = Resource->GetD3D12ResourceDesc().Format;
		}
		return DsvFormat;
	}
	DXGI_SAMPLE_DESC FDX12RenderTarget2D::GetSampleDesc() const
	{
		DXGI_SAMPLE_DESC SampleDesc = { 1, 0 };
		for (int i = 0; i <= m_ColoTexture.size(); ++i)
		{
			auto* Texture = static_cast<FDX12Texture2D*>(m_ColoTexture[i].Texture);
			if (Texture)
			{
				auto Resource = Texture->GetResource();
				SampleDesc = Resource->GetD3D12ResourceDesc().SampleDesc;
				break;
			}
		}

		return SampleDesc;
	}
}
