#include "DX12RenderTargetCube.h"
#include "DX12RenderTarget2D.h"
#include "DX12Device.h"
#include "DX12CommandList.h"
#include "DX12TextureCube.h"
#include "Colors.h"
#include "Render/RHI/GraphicDevice.h"


namespace Zero
{
	FDX12RenderTargetCube::FDX12RenderTargetCube()
		: FRenderTargetCube()
	{
	}

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
				.Width = m_RenderTargetCubeDesc.Size,
				.Height = m_RenderTargetCubeDesc.Size,
				.ArraySize = 6,
				.MipLevels = Desc.MipLevels,
				.ResourceBindFlags = EResourceBindFlag::ShaderResource | EResourceBindFlag::RenderTarget,
				.Format = Desc.TextureFormat,
			};
			std::string ColorTexturName = std::format("{0}_Color", Desc.RenderTargetName);
			m_TextureColorCubemap = 
			{
				.Texture = new FDX12TextureCube(ColorTexturName, TextureDesc),
				.ViewID = 0,
				.ClearValue = FTextureClearValue{},
				.bClearColor = true
			};
		}


		if (m_RenderTargetCubeDesc.bRenderDepth)
		{
			{
				D3D12_RESOURCE_DESC DepthDesc = {
					.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					.Alignment = 0,
					.Width = m_RenderTargetCubeDesc.Size,
					.Height = m_RenderTargetCubeDesc.Size,
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
				.Width = m_RenderTargetCubeDesc.Size,
				.Height = m_RenderTargetCubeDesc.Size,
				.ArraySize = 6,
				.ResourceBindFlags = EResourceBindFlag::ShaderResource | EResourceBindFlag::DepthStencil,
				.Format = EResourceFormat::D24_UNORM_S8_UINT,
			};

			std::string DepthTexturName = std::format("{0}_Depth", Desc.RenderTargetName);
			m_TextureDepthCubemap = {
				.Texture = new FDX12TextureCube(DepthTexturName, TextureDesc),
				.ViewID = 0,
				.ClearValue = FTextureClearValue{},
				.bClearColor = true
			};
		}
	}


	void FDX12RenderTargetCube::Resize(uint32_t Width, uint32_t Height)
	{
		m_RenderTargetCubeDesc.Size = Width;
	}

	void FDX12RenderTargetCube::SetViewportRect(uint32_t Mip)
	{
		// Set Viewport
		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = (float)(m_RenderTargetCubeDesc.Size >> Mip);
		m_ViewPort.Height = (float)(m_RenderTargetCubeDesc.Size >> Mip);
		m_ViewPort.MaxDepth = 1.0f;
		m_ViewPort.MinDepth = 0.0f;

		// Set Scissor Rect
		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = (m_RenderTargetCubeDesc.Size >> Mip);
		m_ScissorRect.bottom = (m_RenderTargetCubeDesc.Size >> Mip);
	}



	void FDX12RenderTargetCube::Bind(FCommandListHandle CommandListHandle)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		if (m_TextureColorCubemap.Texture != nullptr)
		{
			FDX12TextureCube* DX12TextureCubemap = static_cast<FDX12TextureCube*>(m_TextureColorCubemap.Texture);

			CommandList->TransitionBarrier(DX12TextureCubemap->GetResource()->GetD3DResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
		
		if (m_TextureDepthCubemap.Texture != nullptr)
		{
			FDX12TextureCube* DX12TextureCubemap = static_cast<FDX12TextureCube*>(m_TextureDepthCubemap.Texture);
			CommandList->TransitionBarrier(DX12TextureCubemap->GetResource()->GetD3DResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
		}
	}

	void FDX12RenderTargetCube::SetRenderTarget(FCommandListHandle CommandListHandle, uint32_t FaceIndex, uint32_t SubResource /*= -1*/)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		SetViewportRect(SubResource == -1 ? 0 : SubResource);
		CommandList->GetD3D12CommandList()->RSSetViewports(1, &m_ViewPort);
		CommandList->GetD3D12CommandList()->RSSetScissorRects(1, &m_ScissorRect);

		FDX12TextureCube* ColorCubemap = static_cast<FDX12TextureCube*>(m_TextureColorCubemap.Texture);
		FDX12RenderTargetView* RtvPtr = m_TextureColorCubemap.Texture != nullptr ? static_cast<FDX12RenderTargetView*>(ColorCubemap->GetRTV(FaceIndex, SubResource)) : nullptr;
		auto* DsvTexture = static_cast<FDX12TextureCube*>(m_TextureDepthCubemap.Texture);
		FDX12DepthStencilView* DsvPtr = m_TextureDepthCubemap.Texture != nullptr ? static_cast<FDX12DepthStencilView*>(DsvTexture->GetDSV(FaceIndex)) : nullptr;

		if (RtvPtr)
		{
			float Color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			if (FaceIndex == 0)
			{
				Color[0] = 1.0f;
			}
			else if (FaceIndex == 1)
			{
				Color[1] = 1.0f;
			}
			else if (FaceIndex == 2)
			{
				Color[2] = 1.0f;
			}
			else if (FaceIndex == 3)
			{
				Color[0] = 1.0f;
				Color[1] = 1.0f;
			}
			else if (FaceIndex == 4)
			{
				Color[0] = 1.0f;
				Color[2] = 1.0f;
			}
			else if (FaceIndex == 5)
			{
				Color[1] = 1.0f;
				Color[2] = 1.0f;
			}
			CommandList->GetD3D12CommandList()->ClearRenderTargetView(RtvPtr->GetDescriptorHandle(), Color, 0, nullptr);	
		}

		if (DsvPtr)
		{
			CommandList->GetD3D12CommandList()->ClearDepthStencilView(DsvPtr->GetDescriptorHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		}

		if (RtvPtr)
		{
			CommandList->GetD3D12CommandList()->OMSetRenderTargets(
				1,
				&RtvPtr->GetDescriptorHandle(),
				false,
				DsvPtr ? &DsvPtr->GetDescriptorHandle() : nullptr
			);
		}
		else
		{
			CommandList->GetD3D12CommandList()->OMSetRenderTargets(0, nullptr, false, DsvPtr ? &DsvPtr->GetDescriptorHandle() : nullptr);
		}
	}

	void FDX12RenderTargetCube::UnBind(FCommandListHandle CommandListHandle, uint32_t FaceIndex, uint32_t SubResource /*= -1*/)
	{
		auto  CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		if (m_TextureColorCubemap.Texture != nullptr)
		{
			FDX12TextureCube* ColorCubemap = static_cast<FDX12TextureCube*>(m_TextureColorCubemap.Texture);
			CommandList->TransitionBarrier(ColorCubemap->GetResource()->GetD3DResource(), D3D12_RESOURCE_STATE_COMMON);
		}
		
		if (m_TextureDepthCubemap.Texture != nullptr)
		{
			FDX12TextureCube* DsvCubemap = static_cast<FDX12TextureCube*>(m_TextureDepthCubemap.Texture);
			CommandList->TransitionBarrier(DsvCubemap->GetResource()->GetD3DResource(), D3D12_RESOURCE_STATE_COMMON);
		}
	}
}
