#include "DX12TextureCube.h"
#include "DX12Device.h"
#include "DX12CommandQueue.h"
#include "DX12CommandList.h"
#include "Utils.h"


namespace Zero
{
	FDX12TextureCube::FDX12TextureCube(const std::string& TextureName, const FTextureDesc& Desc)
		:FTextureCube(Desc)
	{
		if (HasAnyFlag(m_TextureDesc.ResourceBindFlags, EResourceBindFlag::RenderTarget | EResourceBindFlag::DepthStencil))
		{
			D3D12_HEAP_TYPE D3DHeapType = D3D12_HEAP_TYPE_DEFAULT;
			D3D12_RESOURCE_DESC ResourceDesc = FDX12Utils::ConvertResourceDesc(m_TextureDesc);

			D3D12_CLEAR_VALUE D3DClearValue
			{
				.Format = ResourceDesc.Format
			};
			if (HasAnyFlag(m_TextureDesc.ResourceBindFlags, EResourceBindFlag::DepthStencil))
			{
				D3DClearValue.DepthStencil = { m_TextureDesc.ClearValue.DepthStencil.Depth, m_TextureDesc.ClearValue.DepthStencil.Stencil };
			}
			else
			{
				D3DClearValue.Color[0] = m_TextureDesc.ClearValue.Color.Color[0];
				D3DClearValue.Color[1] = m_TextureDesc.ClearValue.Color.Color[1];
				D3DClearValue.Color[2] = m_TextureDesc.ClearValue.Color.Color[2];
				D3DClearValue.Color[3] = m_TextureDesc.ClearValue.Color.Color[3];
			}

			ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
			ComPtr<ID3D12Resource> D3DResource;
			ThrowIfFailed(
				D3DDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3DHeapType),
					D3D12_HEAP_FLAG_NONE,
					&ResourceDesc,
					D3D12_RESOURCE_STATE_COMMON,
					&D3DClearValue,
					IID_PPV_ARGS(D3DResource.GetAddressOf())
				)
			);
			m_ResourceLocation.m_UnderlyingResource = CreateRef<FDX12Resource>(TextureName, D3DResource, &D3DClearValue);
			m_ResourceLocation.SetType(FResourceLocation::EResourceLocationType::StandAlone);
		}
		else
		{
			D3D12_RESOURCE_DESC ResourceDesc = FDX12Utils::ConvertResourceDesc(m_TextureDesc);

			D3D12_CLEAR_VALUE D3DClearValue
			{
				.Format = ResourceDesc.Format
			};
			D3DClearValue.Color[0] = m_TextureDesc.ClearValue.Color.Color[0];
			D3DClearValue.Color[1] = m_TextureDesc.ClearValue.Color.Color[1];
			D3DClearValue.Color[2] = m_TextureDesc.ClearValue.Color.Color[2];
			D3DClearValue.Color[3] = m_TextureDesc.ClearValue.Color.Color[3];


			auto* TextureResourceAllocator = FDX12Device::Get()->GetTextureResourceAllocator();
			TextureResourceAllocator->AllocTextureResource(TextureName, ResourceDesc, m_ResourceLocation);
		}
		FResourceStateTracker::AddGlobalResourceState(m_ResourceLocation.GetResource()->GetD3DResource().Get(), D3D12_RESOURCE_STATE_COMMON);
		CreateViews(m_TextureDesc.MipLevels);
	}

	FDX12TextureCube::FDX12TextureCube(const std::string& TextureName, Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT], bool bRenderDepth)
		: FTextureCube()
		, m_bRenderDepth(bRenderDepth)
	{
		m_TextureDesc.Width = ImageData[0]->GetWidth();
		m_TextureDesc.Height = ImageData[0]->GetHeight();
		m_TextureDesc.ArraySize = CUBEMAP_TEXTURE_CNT;
		m_TextureDesc.Format = EResourceFormat::R8G8B8A8_UNORM;
		CORE_ASSERT(ImageData[0]->GetData() != nullptr, "Image has no data!");

		FDX12Device::Get()->GetInitWorldCommandList()->AllocateTextureCubemapResource(TextureName, m_TextureDesc, m_ResourceLocation, ImageData);
		CreateViews(m_TextureDesc.MipLevels);
	}


	void FDX12TextureCube::Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize)
	{
		/*
		if (m_D3DResource)
		{
			CD3DX12_RESOURCE_DESC ResDesc(m_D3DResource->GetDesc());
			
			ResDesc.Width = std::max(Width, 1u);
			ResDesc.Height = std::max(Height, 1u);
			ResDesc.DepthOrArraySize = DepthOrArraySize;
			ResDesc.MipLevels = ResDesc.SampleDesc.Count > 1 ? 1 : 0;
			
			m_D3DResource.Reset();
			ThrowIfFailed(
				FDX12Device::Get()->GetDevice()->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &ResDesc,
					D3D12_RESOURCE_STATE_COMMON, m_D3DClearValue.get(), IID_PPV_ARGS(&m_D3DResource)
				)
			);

			// Retain the name of the resource if one was already specified.
			m_D3DResource->SetName(m_ResourceName.c_str());

			FResourceStateTracker::AddGlobalResourceState(m_D3DResource.Get(), D3D12_RESOURCE_STATE_COMMON);
			
			CreateViews();
		}
		*/
	}

	void FDX12TextureCube::CreateViews(uint32_t MipLevels)
	{
		if (m_ResourceLocation.GetResource()->GetD3DResource())
		{
			auto D3DDevice = FDX12Device::Get()->GetDevice();

			CD3DX12_RESOURCE_DESC Desc(m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc());

			// Create RTV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && m_ResourceLocation.GetResource()->CheckRTVSupport())
			{
				m_RTVs.resize(6 * MipLevels);
				uint32_t Index = 0;
				for (uint32_t i = 0; i < 6; ++i)
				{
					for (uint32_t Mip = 0; Mip < MipLevels; ++Mip)
					{
						D3D12_RENDER_TARGET_VIEW_DESC RtvDesc = {
							.Format = m_ResourceLocation.GetResource()->GetD3D12ResourceDesc().Format,
							.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY,
							.Texture2DArray =
							{
								.MipSlice = Mip,
								.FirstArraySlice = UINT(i),
								.ArraySize = 1,
								.PlaneSlice = 0,
							},
						};
						auto Ptr = CreateScope<FDX12RenderTargetView>(m_ResourceLocation.GetResource(), &RtvDesc);
						m_RTVs[Index++] = std::move(Ptr);
					}
				}
			}

			// Create DSV	
			if (m_bRenderDepth && (Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && m_ResourceLocation.GetResource()->CheckDSVSupport())
			{
				for (uint32_t i = 0; i < 6; ++i)
				{
					m_DSVs.resize(6);
					D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc = {
						.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
						.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY,
						.Flags = D3D12_DSV_FLAG_NONE,
						.Texture2DArray =
						{
							.MipSlice = 0,
							.FirstArraySlice = UINT(i),
							.ArraySize = 1,
						},
					};
					auto Ptr = CreateScope<FDX12DepthStencilView>(m_ResourceLocation.GetResource(), &DsvDesc);
					m_DSVs[i] = std::move(Ptr);
				}
			}
			// Create SRV
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && m_ResourceLocation.GetResource()->CheckSRVSupport())
			{
				m_SRVs.resize(1);
				D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {
					.Format = Desc.Format,
					.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE,
					.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
					.Texture2D = 
					{
						.MostDetailedMip = 0,
						.MipLevels = MipLevels,
						.ResourceMinLODClamp = 0.0f,
					}
				};
				auto Ptr = CreateScope<FDX12ShaderResourceView>(m_ResourceLocation.GetResource(), &SrvDesc);
				m_SRVs[0] = std::move(Ptr);
			}
		}
	}


	Zero::FResourceView* FDX12TextureCube::GetRTV(uint32_t FaceIndex /*= 0*/, uint32_t SubResourceIndex /*= -1*/) const
	{
		uint32_t Index = SubResourceIndex == -1 ? FaceIndex : m_TextureDesc.MipLevels * FaceIndex + SubResourceIndex;
		return m_RTVs[Index].get();
	}

	FResourceView* FDX12TextureCube::GetDSV(uint32_t ViewID /*= 0*/) const
	{
		return m_DSVs[ViewID].get();
	}

	FResourceView* FDX12TextureCube::GetSRV(uint32_t ViewID /*= 0*/) const
	{
		return m_SRVs[ViewID].get();
	}

}
