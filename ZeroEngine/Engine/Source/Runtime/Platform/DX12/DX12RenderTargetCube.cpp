#include "DX12RenderTargetCube.h"
#include "DX12RenderTarget2D.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include "Render/RendererAPI.h"
#include "Colors.h"

namespace Zero
{
	FDX12RenderTargetCube::FDX12RenderTargetCube(const FRenderTargetCubeDesc& Desc)
		: FRenderTargetCube(Desc)
	{
		DXGI_FORMAT DxgiFormat = FDX12RHItConverter::GetTextureFormatByDesc(Desc.TextureFormat);
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

		CD3DX12_CLEAR_VALUE OptClear;
		OptClear.Format = DxgiFormat;
		memcpy(OptClear.Color, DirectX::Colors::Transparent, 4 * sizeof(float));
		std::string ColorTexturName = std::format("{0}_Color", Desc.RenderTargetName);
		m_TextureColorCubemap = CreateRef<FDX12TextureCubemap>(ColorTexturName, TexDesc, &OptClear);

		if (m_bRenderDepth)
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
			std::string DepthTexturName = std::format("{0}_Depth", Desc.RenderTargetName);
			m_TextureDepthCubemap = CreateRef<FDX12TextureCubemap>(DepthTexturName, DepthDesc);
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

	void FDX12RenderTargetCube::SetRenderTarget(uint32_t Index)
	{
		auto  CommandList = FDX12Device::Get()->GetRenderCommandList();
		FDX12TextureCubemap* DX12TextureCubemap = static_cast<FDX12TextureCubemap*>(m_TextureColorCubemap.get());
		D3D12_CPU_DESCRIPTOR_HANDLE RTV = DX12TextureCubemap->GetRenderTargetView(Index);
		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		CommandList->GetD3D12CommandList()->ClearRenderTargetView(RTV, ClearColor, 0, nullptr);
		CommandList->GetD3D12CommandList()->OMSetRenderTargets(1, &RTV, true, nullptr);
	}

	void FDX12RenderTargetCube::Bind()
	{
		auto  CommandList = FDX12Device::Get()->GetRenderCommandList();
		FDX12TextureCubemap* DX12TextureCubemap = static_cast<FDX12TextureCubemap*>(m_TextureColorCubemap.get());
		CommandList->GetD3D12CommandList()->RSSetViewports(1, &m_ViewPort);
		CommandList->GetD3D12CommandList()->RSSetScissorRects(1, &m_ScissorRect);
		CommandList->TransitionBarrier(DX12TextureCubemap->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	void FDX12RenderTargetCube::UnBind()
	{
		auto  CommandList = FDX12Device::Get()->GetRenderCommandList();
		FDX12TextureCubemap* DX12TextureCubemap = static_cast<FDX12TextureCubemap*>(m_TextureColorCubemap.get());
		CommandList->TransitionBarrier(DX12TextureCubemap->GetD3DResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}

}
