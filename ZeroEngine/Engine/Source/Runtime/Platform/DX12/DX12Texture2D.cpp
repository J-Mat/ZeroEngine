#include "DX12Texture2D.h"
#include "DX12Device.h"
#include "DX12CommandQueue.h"
#include "DX12CommandList.h"
#include "Utils.h"


namespace Zero
{
	D3D12_CLEAR_VALUE FDX12Texture2D::GetClearValue()
	{
		D3D12_CLEAR_VALUE ClearValue{};
		if (HasAnyFlag(m_TextureDesc.ResourceBindFlags, EResourceBindFlag::DepthStencil) 
			&& m_TextureDesc.ClearValue.EActiveMember == FTextureClearValue::EActiveMember::DepthStencil)
		{
			ClearValue.DepthStencil.Depth = m_TextureDesc.ClearValue.DepthStencil.Depth;
			ClearValue.DepthStencil.Stencil = m_TextureDesc.ClearValue.DepthStencil.Stencil;
			switch (m_TextureDesc.Format)
			{
			case EResourceFormat::R16_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_D16_UNORM;
				break;
			case EResourceFormat::R32_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_D32_FLOAT;
				break;
			case EResourceFormat::R24G8_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				break;
			case EResourceFormat::R32G8X24_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				break;
			default:
				ClearValue.Format = FDX12Utils::ConvertResourceFormat(m_TextureDesc.Format);
				break;
			}
		}
		else if (HasAnyFlag(m_TextureDesc.ResourceBindFlags, EResourceBindFlag::RenderTarget) 
			&& m_TextureDesc.ClearValue.EActiveMember == FTextureClearValue::EActiveMember::Color)
		{
			ClearValue.Color[0] = m_TextureDesc.ClearValue.Color.Color[0];
			ClearValue.Color[1] = m_TextureDesc.ClearValue.Color.Color[1];
			ClearValue.Color[2] = m_TextureDesc.ClearValue.Color.Color[2];
			ClearValue.Color[3] = m_TextureDesc.ClearValue.Color.Color[3];
			switch (m_TextureDesc.Format)
			{
			case EResourceFormat::R16_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_R16_UNORM;
				break;
			case EResourceFormat::R32_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case EResourceFormat::R24G8_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case EResourceFormat::R32G8X24_TYPELESS:
				ClearValue.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
				break;
			default:
				ClearValue.Format = FDX12Utils::ConvertResourceFormat(m_TextureDesc.Format);
				break;
			}
		}
		return ClearValue;
	}
	void FDX12Texture2D::AttachHeapTypeAndResourceState(D3D12_HEAP_TYPE& D3DHeapType, D3D12_RESOURCE_STATES& D3DResourceState, D3D12_RESOURCE_DESC& ResourceDesc)
	{
		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		if (m_TextureDesc.HeapType == EResourceUsage::Readback || m_TextureDesc.HeapType == EResourceUsage::Upload)
		{
			UINT64 RequiredSize = 0;
			D3DDevice->GetCopyableFootprints(
				&ResourceDesc, 
				0, 
				1, 
				0, 
				&m_Footprint,
				nullptr, 
				nullptr, 
				&RequiredSize);
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			ResourceDesc.Width = RequiredSize;
			ResourceDesc.Height = 1;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			if (m_TextureDesc.HeapType == EResourceUsage::Readback)
			{
				D3DHeapType = D3D12_HEAP_TYPE_READBACK;
				D3DResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
			}
			else if (m_TextureDesc.HeapType == EResourceUsage::Upload)
			{
				D3DHeapType = D3D12_HEAP_TYPE_UPLOAD;
				D3DResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
			}
		}
	}
	FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, const FTextureDesc Desc)
		: FTexture2D(Desc)
	{
		D3D12_RESOURCE_DESC ResourceDesc
		{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = m_TextureDesc.Width,
			.Height = m_TextureDesc.Height,
			.DepthOrArraySize = (UINT16)m_TextureDesc.ArraySize,
			.MipLevels = m_TextureDesc.MipLevels,
			.Format = FDX12Utils::ConvertResourceFormat(m_TextureDesc.Format),
			.SampleDesc =
			{
				.Count = m_TextureDesc.SampleCount,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = FDX12Utils::BindFlagsByResourceFlags(m_TextureDesc.ResourceBindFlags),
		};
		D3D12_CLEAR_VALUE ClearValue = GetClearValue();
		
		D3D12_RESOURCE_STATES D3DResourceState = FDX12Utils::ConvertToD3DResourceState(m_TextureDesc.InitialState);
		D3D12_HEAP_TYPE D3DHeapType = D3D12_HEAP_TYPE_DEFAULT;
		AttachHeapTypeAndResourceState(D3DHeapType, D3DResourceState, ResourceDesc);

		ID3D12Device* D3DDevice = FDX12Device::Get()->GetDevice();
		m_D3DResource.Reset();
		ThrowIfFailed(
			D3DDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3DHeapType), 
				D3D12_HEAP_FLAG_NONE,
				&ResourceDesc, 
				D3DResourceState, 
				&ClearValue, 
				IID_PPV_ARGS(&m_D3DResource)
			)
		);
		m_D3DResource->SetName(Utils::StringToLPCWSTR(TextureName));
		SetResource(m_D3DResource);
	}

	FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, const FDX12TextureSettings& TextureSettings, const D3D12_CLEAR_VALUE* FTextureClearValue)
		: FTexture2D()
		, FResource(TextureName, TextureSettings.Desc, FTextureClearValue)
	{
		m_TextureDesc.Width = (uint32_t)TextureSettings.Desc.Width;
		m_TextureDesc.Height = (uint32_t)TextureSettings.Desc.Height;
		m_SRVFormat = TextureSettings.SRVFormat;
		m_RTVFormat = TextureSettings.RTVFormat;
		m_DSVFormat = TextureSettings.DSVFormat;
		m_UAVFormat = TextureSettings.UAVFormat;
		CreateViews();
	}


	FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, Ref<FImage> ImageData, bool bNeedMipMap)
		 : FTexture2D(bNeedMipMap)
		,  FResource()
	{
		m_ImageData = ImageData;
		m_TextureDesc.Width = ImageData->GetWidth();
		m_TextureDesc.Height = ImageData->GetHeight();
		CORE_ASSERT(ImageData->GetData() != nullptr, "Image has no data!");
		auto Resource = FDX12Device::Get()->GetInitWorldCommandList()->CreateTextureResource(TextureName, ImageData, m_bNeedMipMap);
		SetResource(Resource->GetD3DResource());
		CreateViews();
	}

    FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, const D3D12_CLEAR_VALUE* FTextureClearValue)
		: FResource(Resource, FTextureClearValue)
	{
		m_TextureDesc.Width = Width;
		m_TextureDesc.Height = Height;
		Resource->SetName(Utils::StringToLPCWSTR(TextureName));
		CreateViews();
	}


	void FDX12Texture2D::Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1)
	{
		if (m_D3DResource)
		{
			CD3DX12_RESOURCE_DESC ResDesc(m_D3DResource->GetDesc());
			
			m_TextureDesc.Width = ResDesc.Width = std::max(Width, 1u);
			m_TextureDesc.Height = ResDesc.Height = std::max(Height, 1u);
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

	void FDX12Texture2D::RegistGuiShaderResource()
	{
		if (m_GuiAllocation.IsNull())
		{
			m_GuiAllocation = FDX12Device::Get()->AllocateGuiDescritor();
		}
		FDX12Device::Get()->GetDevice()->CreateShaderResourceView(
			m_D3DResource.Get(), 
			nullptr,
			m_GuiAllocation.CpuHandle
		);
		m_bHasGuiResource = true;
	}

	UINT64 FDX12Texture2D::GetGuiShaderReseource()
	{
		CORE_ASSERT(m_bHasGuiResource, "Textrue must has gui resource!");
		D3D12_GPU_DESCRIPTOR_HANDLE* Handle = &(m_GuiAllocation.GpuHandle);
		return m_GuiAllocation.GpuHandle.ptr;
	}

	// Get a UAV description that matches the resource description.
	D3D12_UNORDERED_ACCESS_VIEW_DESC FDX12Texture2D::GetUAVDesc(const D3D12_RESOURCE_DESC& ResDesc, UINT MipSlice, UINT ArraySlice, UINT PlaneSlice)
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

	void FDX12Texture2D::CreateViews()
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
				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.Texture2D.MipSlice = 0;
				D3DDevice->CreateDepthStencilView(m_D3DResource.Get(), &dsvDesc,
					m_DepthStencilView.GetDescriptorHandle());
			}
			// Create SRV
			if (((Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport()) || m_SRVFormat != DXGI_FORMAT_UNKNOWN)
			{
				if (m_ShaderResourceView.IsNull())
				{
					m_ShaderResourceView = FDX12Device::Get()->AllocateRuntimeDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
				if (m_ImageData != nullptr)
				{
					std::uint8_t srv_srcs[4];
					for (size_t i = 0; i < m_ImageData->GetChannel(); i++)
						srv_srcs[i] = static_cast<std::uint8_t>(i);
					for (size_t i = m_ImageData->GetChannel(); i < 4; i++)
						srv_srcs[i] = D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1;
					SrvDesc.Shader4ComponentMapping =
						D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(srv_srcs[0], srv_srcs[1], srv_srcs[2], srv_srcs[3]);
				}
				else
				{
					SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				}
				SrvDesc.Format = m_SRVFormat == DXGI_FORMAT_UNKNOWN ? m_D3DResource->GetDesc().Format : m_SRVFormat;
				SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				SrvDesc.Texture2D.MostDetailedMip = 0;
				SrvDesc.Texture2D.MipLevels = 1;
				SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				SrvDesc.Texture2D.PlaneSlice = 0;
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

	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetRTV() const
	{
		return m_RenderTargetView.GetDescriptorHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetDSV() const
	{
		return m_DepthStencilView.GetDescriptorHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetSRV() const
	{
		return m_ShaderResourceView.GetDescriptorHandle();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetUnorderedAccessView(uint32_t mip) const
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}


	DXGI_FORMAT FDX12Texture2D::GetUAVCompatableFormat(DXGI_FORMAT Format)
	{
		DXGI_FORMAT UavFormat = Format;

		switch (Format)
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			UavFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
			UavFormat = DXGI_FORMAT_R32_FLOAT;
			break;
		}

		return UavFormat;
	}
	bool FDX12Texture2D::IsSRGBFormat(DXGI_FORMAT Format)
	{
		switch (Format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return true;
		default:
			return false;
		}

		return false;
	}

	DXGI_FORMAT FDX12Texture2D::GetSRGBFormat(DXGI_FORMAT Format)
	{
		DXGI_FORMAT SrgbFormat = Format;
		switch (Format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			SrgbFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC1_UNORM:
			SrgbFormat = DXGI_FORMAT_BC1_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC2_UNORM:
			SrgbFormat = DXGI_FORMAT_BC2_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC3_UNORM:
			SrgbFormat = DXGI_FORMAT_BC3_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			SrgbFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			SrgbFormat = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC7_UNORM:
			SrgbFormat = DXGI_FORMAT_BC7_UNORM_SRGB;
			break;
		}

		return SrgbFormat;
	}
}
