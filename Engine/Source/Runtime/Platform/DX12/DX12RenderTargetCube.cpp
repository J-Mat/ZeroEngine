#include "DX12RenderTargetCube.h"
#include "DX12RenderTarget2D.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include "DX12TextureCube.h"
#include "Colors.h"


namespace Zero
{
	FDX12RenderTargetCube::FDX12RenderTargetCube(const FRenderTargetCubeDesc& Desc)
		: FRenderTargetCube(Desc)
	{
		/*
		D3D12_RESOURCE_DESC TexDesc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = m_Size,
			.Height = m_Size,
			.DepthOrArraySize = 6,
			.MipLevels = 1,
			.Format = DxgiFormat,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		};
		*/

		{
			FTextureDesc TextureDesc = {
				.Type = ETextureType::TextureType_2D,
				.Width = m_Size,
				.Height = m_Size,
				.ArraySize = 6,
				.ResourceBindFlags = EResourceBindFlag::ShaderResource | EResourceBindFlag::RenderTarget,
				.Format = Desc.TextureFormat,
			};
			std::string ColorTexturName = std::format("{0}_Color", Desc.RenderTargetName);
			m_TextureColorCubemap = CreateRef<FDX12TextureCube>(ColorTexturName, TextureDesc);
		}


		if (m_bRenderDepth)
		{
			{
				D3D12_RESOURCE_DESC DepthDesc = {
					.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					.Alignment = 0,
					.Width = m_Size,
					.Height = m_Size,
					.DepthOrArraySize = 6,
					.MipLevels = 1,
					.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
					.SampleDesc = {
						.Count = 1,
						.Quality = 0
					},
					.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
					.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
				};
			}

			FTextureDesc TextureDesc = {
				.Type = ETextureType::TextureType_2D,
				.Width = m_Size,
				.Height = m_Size,
				.ArraySize = 6,
				.ResourceBindFlags = EResourceBindFlag::ShaderResource | EResourceBindFlag::DepthStencil,
				.Format = EResourceFormat::D24_UNORM_S8_UINT,
			};

			std::string DepthTexturName = std::format("{0}_Depth", Desc.RenderTargetName);
			m_TextureDepthCubemap = CreateRef<FDX12TextureCube>(DepthTexturName, TextureDesc);
		}
		SetViewportRect();
	}
	void FDX12RenderTargetCube::SetViewportRect()
	{
		// Set Viewport
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = (float)m_Size;
		m_ViewPort.Height = (float)m_Size;
		m_ViewPort.MaxDepth = 1.0f;
		m_ViewPort.MinDepth = 0.0f;

		// Set Scissor Rect
		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = m_Size;
		m_ScissorRect.bottom = m_Size;
	}

	void FDX12RenderTargetCube::SetRenderTarget(uint32_t Index, FCommandListHandle CommandListHandle)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		FDX12TextureCube* DX12TextureCubemap = static_cast<FDX12TextureCube*>(m_TextureColorCubemap.get());
		FDX12RenderTargetView* RTV = static_cast<FDX12RenderTargetView*>(DX12TextureCubemap->GetRTV(Index));
		ZMath::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		if (Index == 0)
		{
			ClearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		}
		if (Index == 1)
		{
			ClearColor = { 0.0f, 1.0f, 0.0f, 1.0f };
		}
		if (Index == 2)
		{
			ClearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
		}
		if (Index == 3)
		{
			ClearColor = { 1.0f, 1.0f, 0.0f, 1.0f };
		}
		if (Index == 4)
		{
			ClearColor = { 1.0f, 0.0f, 1.0f, 1.0f };
		}
		if (Index == 5)
		{
			ClearColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
		float Color[4] = { ClearColor[0],ClearColor[1], ClearColor[2], ClearColor[3]};

		CommandList->GetD3D12CommandList()->ClearRenderTargetView(RTV->GetDescriptorHandle(),Color, 0, nullptr);
		CommandList->GetD3D12CommandList()->OMSetRenderTargets(1, &RTV->GetDescriptorHandle(), true, nullptr);
	}

	void FDX12RenderTargetCube::Bind(FCommandListHandle CommandListHandle)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		FDX12TextureCube* DX12TextureCubemap = static_cast<FDX12TextureCube*>(m_TextureColorCubemap.get());
		CommandList->GetD3D12CommandList()->RSSetViewports(1, &m_ViewPort);
		CommandList->GetD3D12CommandList()->RSSetScissorRects(1, &m_ScissorRect);
		CommandList->TransitionBarrier(DX12TextureCubemap->GetResource()->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void FDX12RenderTargetCube::UnBind(FCommandListHandle CommandListHandle)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		FDX12TextureCube* DX12TextureCubemap = static_cast<FDX12TextureCube*>(m_TextureColorCubemap.get());
		CommandList->TransitionBarrier(DX12TextureCubemap->GetResource()->GetD3DResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

}
