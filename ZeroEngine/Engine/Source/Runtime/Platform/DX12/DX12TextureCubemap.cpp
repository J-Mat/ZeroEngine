#include "DX12TextureCubemap.h"
#include "DX12Device.h"
#include "DX12CommandQueue.h"
#include "DX12CommandList.h"


namespace Zero
{
	DXGI_FORMAT FDX12TextureCubemap::GetFormatByDesc(ETextureFormat Format)
	{
		switch (Format)
		{
		case Zero::ETextureFormat::None:
			return DXGI_FORMAT_UNKNOWN;
		case Zero::ETextureFormat::R8G8B8:
			return DXGI_FORMAT_R8G8_UNORM;
		case Zero::ETextureFormat::R8G8B8A8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Zero::ETextureFormat::INT32:
			return DXGI_FORMAT_R32_UINT;
		case Zero::ETextureFormat::R32G32B32A32:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Zero::ETextureFormat::DEPTH32F:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			break;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
	FDX12TextureCubemap::FDX12TextureCubemap(const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* clearValue)
		: IResource(ResourceDesc, clearValue)
	{
		m_Width = (uint32_t)ResourceDesc.Width;
		m_Height = (uint32_t)ResourceDesc.Height;
		CreateViews();
	}

	FDX12TextureCubemap::FDX12TextureCubemap(Ref<FImage> ImageData[CUBEMAP_TEXTURE_CNT])
		:  IResource()
	{
		m_Width = ImageData[0]->GetWidth();
		m_Height = ImageData[0]->GetHeight();
		CORE_ASSERT(ImageData[0]->GetData() != nullptr, "Image has no data!");

		/*
		m_EachFaceSize = ImageData[0]->GetBufferSize();
		m_Data = (char*)malloc(m_EachFaceSize * CUBEMAP_TEXTURE_CNT);
		uint32_t Offset = 0;
		for (size_t i = 0; i < CUBEMAP_TEXTURE_CNT; ++i)
		{
			memcpy(&m_Data[Offset], ImageData[i]->GetData(), m_EachFaceSize);
			Offset += m_EachFaceSize;
		}
		*/
		auto Resource = 
			FDX12Device::Get()->GetInitWorldCommandList()->CreateTextureCubemapResource(
				ImageData,
				m_Width, 
				m_Height,
				ImageData[0]->GetChannel()
			);
		SetResource(Resource);
		CreateViews();
	}

	FDX12TextureCubemap::FDX12TextureCubemap(ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, const D3D12_CLEAR_VALUE* ClearValue)
		: IResource(Resource, ClearValue)
	{
		m_Width = Width;
		m_Height = Height;
		CreateViews();
	}

	void FDX12TextureCubemap::Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize)
	{
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
	}

	// Get a UAV description that matches the resource description.
	D3D12_UNORDERED_ACCESS_VIEW_DESC FDX12TextureCubemap::GetUAVDesc(const D3D12_RESOURCE_DESC& ResDesc, UINT MipSlice, UINT ArraySlice, UINT PlaneSlice)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc = {};
		UavDesc.Format = ResDesc.Format;

		switch (ResDesc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (ResDesc.DepthOrArraySize > 1)
			{
				UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
				UavDesc.Texture1DArray.ArraySize = ResDesc.DepthOrArraySize - ArraySlice;
				UavDesc.Texture1DArray.FirstArraySlice = ArraySlice;
				UavDesc.Texture1DArray.MipSlice = MipSlice;
			}
			else
			{
				UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
				UavDesc.Texture1D.MipSlice = MipSlice;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (ResDesc.DepthOrArraySize > 1)
			{
				UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				UavDesc.Texture2DArray.ArraySize = ResDesc.DepthOrArraySize - ArraySlice;
				UavDesc.Texture2DArray.FirstArraySlice = ArraySlice;
				UavDesc.Texture2DArray.PlaneSlice = PlaneSlice;
				UavDesc.Texture2DArray.MipSlice = MipSlice;
			}
			else
			{
				UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				UavDesc.Texture2D.PlaneSlice = PlaneSlice;
				UavDesc.Texture2D.MipSlice = MipSlice;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			UavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			UavDesc.Texture3D.WSize = ResDesc.DepthOrArraySize - ArraySlice;
			UavDesc.Texture3D.FirstWSlice = ArraySlice;
			UavDesc.Texture3D.MipSlice = MipSlice;
			break;
		default:
			throw std::exception("Invalid resource dimension.");
		}

		return UavDesc;
	}

	void FDX12TextureCubemap::CreateViews()
	{
		if (m_D3DResource)
		{
			auto D3DDevice = FDX12Device::Get()->GetDevice();
			
			CD3DX12_RESOURCE_DESC Desc(m_D3DResource->GetDesc());

			// Create RTV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport())
			{
				if (m_RenderTargetView.IsNull())
				{
					m_RenderTargetView = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				}
				D3DDevice->CreateRenderTargetView(m_D3DResource.Get(), nullptr, m_RenderTargetView.GetDescriptorHandle());
			}

			// Create DSV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport())
			{
				if (m_DepthStencilView.IsNull())
				{
					m_DepthStencilView = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
				}
				D3DDevice->CreateDepthStencilView(m_D3DResource.Get(), nullptr,
					m_DepthStencilView.GetDescriptorHandle());
			}
			// Create SRV
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
			{
				if (m_ShaderResourceView.IsNull())
				{
					m_ShaderResourceView = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
				SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				SrvDesc.Format = Desc.Format;
				SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				SrvDesc.Texture2D.MostDetailedMip = 0;
				SrvDesc.Texture2D.MipLevels = 1;
				SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				D3DDevice->CreateShaderResourceView(m_D3DResource.Get(), &SrvDesc,
					m_ShaderResourceView.GetDescriptorHandle());
			}
			// Create UAV for each mip (only supported for 1D and 2D textures).
			if ((Desc.Flags & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0 && CheckUAVSupport() &&
				Desc.DepthOrArraySize == 1)
			{
				if (m_UnorderedAccessView.IsNull())
				{
					m_UnorderedAccessView = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Desc.MipLevels);
				}
				for (int i = 0; i < Desc.MipLevels; ++i)
				{
					auto uavDesc = GetUAVDesc(Desc, i);
					D3DDevice->CreateUnorderedAccessView(m_D3DResource.Get(), nullptr, &uavDesc,
						m_UnorderedAccessView.GetDescriptorHandle(i));
				}
			}
		}
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12TextureCubemap::GetRenderTargetView() const
	{
		return m_RenderTargetView.GetDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12TextureCubemap::GetDepthStencilView() const
	{
		return m_DepthStencilView.GetDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12TextureCubemap::GetShaderResourceView() const
	{
		return m_ShaderResourceView.GetDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12TextureCubemap::GetUnorderedAccessView(uint32_t mip) const
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}
}
