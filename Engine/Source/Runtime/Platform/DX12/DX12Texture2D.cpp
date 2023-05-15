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
			&& m_TextureDesc.ClearValue.ActiveMember == FTextureClearValue::ActiveMember::DepthStencil)
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
			&& m_TextureDesc.ClearValue.ActiveMember == FTextureClearValue::ActiveMember::Color)
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
	void FDX12Texture2D::AttachHeapTypeAndResourceState(D3D12_HEAP_TYPE& D3DHeapType, D3D12_RESOURCE_DESC& ResourceDesc)
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
			}
			else if (m_TextureDesc.HeapType == EResourceUsage::Upload)
			{
				D3DHeapType = D3D12_HEAP_TYPE_UPLOAD;
			}
		}
	}

	FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, const FTextureDesc& Desc, bool bCreateTextureView /*= true*/)
		: FTexture2D(Desc)
	{
		if (HasAnyFlag(m_TextureDesc.ResourceBindFlags, EResourceBindFlag::RenderTarget | EResourceBindFlag::DepthStencil))
		{
			D3D12_HEAP_TYPE D3DHeapType = D3D12_HEAP_TYPE_DEFAULT;
			D3D12_RESOURCE_DESC ResourceDesc = FDX12Utils::ConvertResourceDesc(m_TextureDesc);
			AttachHeapTypeAndResourceState(D3DHeapType, ResourceDesc);

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
		if (bCreateTextureView)
		{
			CreateViews();
		}
	}

	FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, Ref<FImage> ImageData, bool bNeedMipMap)
		 : FTexture2D(ImageData, bNeedMipMap)
	{
		m_TextureDesc.Width = (uint64_t)ImageData->GetWidth();
		m_TextureDesc.Height = ImageData->GetHeight();
		m_TextureDesc.MipLevels = bNeedMipMap ? ZMath::min(ZMath::CalLog2Interger((uint32_t)m_TextureDesc.Width), ZMath::CalLog2Interger(m_TextureDesc.Height)) + 1 : 1;
		m_TextureDesc.Depth = 1;
		m_TextureDesc.Format = EResourceFormat::R8G8B8A8_UNORM;
		m_TextureDesc.SampleCount = 1;
		m_TextureDesc.ResourceBindFlags = EResourceBindFlag::UnorderedAccess;

		CORE_ASSERT(ImageData->GetData() != nullptr, "Image has no data!");
		//auto Resource = FDX12Device::Get()->GetInitWorldCommandList()->CreateTextureResource(TextureName, ImageData, m_bNeedMipMap);

		FDX12Device::Get()->GetInitWorldCommandList()->AllocateTextureResource(TextureName, m_TextureDesc, m_ResourceLocation, ImageData, bNeedMipMap);
		CreateViews();
	}

	FDX12Texture2D::FDX12Texture2D(const std::string& TextureName, ComPtr<ID3D12Resource> Resource, uint32_t Width, uint32_t Height, const D3D12_CLEAR_VALUE* FTextureClearValue)
		: FTexture2D(nullptr)
	{
		m_ResourceLocation.ReleaseResource();
		m_ResourceLocation.m_UnderlyingResource = CreateRef<FDX12Resource>(TextureName, Resource, FTextureClearValue);
		m_ResourceLocation.SetType(FResourceLocation::EResourceLocationType::StandAlone);
		m_TextureDesc.Width = Width;
		m_TextureDesc.Height = Height;
		CreateViews();
	}


	FDX12Texture2D::~FDX12Texture2D()
	{
		CORE_LOG_INFO("Delete Texture {0}", Utils::WString2String(m_ResourceLocation.GetResource()->GetName()));
	}

	void FDX12Texture2D::SetName(const std::string& Name)
	{
		m_ResourceLocation.GetResource()->SetName(Utils::String2WString(Name));
	}

	void FDX12Texture2D::Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1)
	{
		Ref<FDX12Resource> Resource = m_ResourceLocation.GetResource();
		if (m_ResourceLocation.GetResource()->GetD3DResource())
		{
			if (m_ResourceLocation.m_ResourceLocationType == FResourceLocation::EResourceLocationType::SubAllocation)
			{
				m_ResourceLocation.ReleaseResource();
				CD3DX12_RESOURCE_DESC ResDesc(m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc());
				m_TextureDesc.Width = std::max(Width, 1u);
				m_TextureDesc.Height = std::max(Height, 1u);
				m_TextureDesc.MipLevels = ResDesc.SampleDesc.Count > 1 ? 1 : 0;

				D3D12_RESOURCE_DESC ResourceDesc = FDX12Utils::ConvertResourceDesc(m_TextureDesc);

				auto* TextureResourceAllocator = FDX12Device::Get()->GetTextureResourceAllocator();
				TextureResourceAllocator->AllocTextureResource(Utils::WString2String(Resource->GetName()), ResourceDesc, m_ResourceLocation);

				FResourceStateTracker::AddGlobalResourceState(m_ResourceLocation.GetResource()->GetD3DResource().Get(), D3D12_RESOURCE_STATE_COMMON);
			}
			else if (m_ResourceLocation.m_ResourceLocationType == FResourceLocation::EResourceLocationType::StandAlone)
			{
				auto Resource = m_ResourceLocation.GetResource();
				auto ResourceName = Resource->GetName();
				ComPtr<ID3D12Resource> D3DResource = Resource->GetD3DResource();
				CD3DX12_RESOURCE_DESC ResDesc(D3DResource->GetDesc());

				m_TextureDesc.Width = ResDesc.Width = std::max(Width, 1u);
				m_TextureDesc.Height = ResDesc.Height = std::max(Height, 1u);
				ResDesc.DepthOrArraySize = DepthOrArraySize;
				ResDesc.MipLevels = ResDesc.SampleDesc.Count > 1 ? 1 : 0;

				CD3DX12_CLEAR_VALUE ClearValue = {};
				CD3DX12_CLEAR_VALUE* ClearValuePtr = nullptr;
				if (HasAnyFlag(m_TextureDesc.ResourceBindFlags, EResourceBindFlag::DepthStencil))
				{
					FLOAT Depth = 1.0f;
					UINT8 Stencil = 0;
					ClearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, Depth, Stencil);
					ClearValuePtr = &ClearValue;
				}

				ComPtr<ID3D12Resource> NewResource;
				ThrowIfFailed(
					FDX12Device::Get()->GetDevice()->CreateCommittedResource(
						&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &ResDesc,
						D3D12_RESOURCE_STATE_COMMON, ClearValuePtr, IID_PPV_ARGS(&NewResource)
					)
				);
				m_ResourceLocation.m_UnderlyingResource.reset();
				m_ResourceLocation.m_UnderlyingResource = CreateRef<FDX12Resource>(Utils::WString2String(ResourceName), NewResource);
				// Retain the name of the resource if one was already specified.

				FResourceStateTracker::AddGlobalResourceState(m_ResourceLocation.GetResource()->GetD3DResource().Get(), D3D12_RESOURCE_STATE_COMMON);
			}
			
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
			m_ResourceLocation.GetResource()->GetD3DResource().Get(), 
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
		if (m_ResourceLocation.GetResource()->GetD3DResource())
		{
			auto D3DDevice = FDX12Device::Get()->GetDevice();
			
			CD3DX12_RESOURCE_DESC Desc(m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc());

			// Create RTV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && m_ResourceLocation.GetResource()->CheckRTVSupport())
			{
				m_RTVs.resize(1);
				auto ptr = CreateScope<FDX12RenderTargetView>(m_ResourceLocation.GetResource(), nullptr);
				m_RTVs[0] = std::move(ptr);
			}

			// Create DSV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && m_ResourceLocation.GetResource()->CheckDSVSupport())
			{
				D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
				DsvDesc.Flags = D3D12_DSV_FLAG_NONE;
				DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				DsvDesc.Texture2D.MipSlice = 0;
				//D3DDevice->CreateDepthStencilView(m_ResourceLocation.GetResource()->GetD3DResource().Get(), &DsvDesc, m_DepthStencilView.GetDescriptorHandle());
				
				m_DSVs.resize(1);
				auto Dsv = CreateScope<FDX12DepthStencilView>(m_ResourceLocation.GetResource(), &DsvDesc);
				m_DSVs[0] = std::move(Dsv);
			}
			// Create SRV
			m_SRVFormat = GetSRVFormat(Desc.Format);
			if (((Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && m_ResourceLocation.GetResource()->CheckSRVSupport()) || m_SRVFormat != DXGI_FORMAT_UNKNOWN)
			{
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
				SrvDesc.Format = m_SRVFormat == DXGI_FORMAT_UNKNOWN ? m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc().Format : m_SRVFormat;
				SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				SrvDesc.Texture2D.MostDetailedMip = 0;
				SrvDesc.Texture2D.MipLevels = Desc.MipLevels;
				SrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				SrvDesc.Texture2D.PlaneSlice = 0;
				m_SRVs.resize(1);
				auto Srv = CreateScope<FDX12ShaderResourceView>(m_ResourceLocation.GetResource(), &SrvDesc);
				m_SRVs[0] = std::move(Srv);
				
			}
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && m_ResourceLocation.GetResource()->CheckSRVSupport())
			{
				m_bHasGuiResource = false;
				RegistGuiShaderResource();
			}
			// Create UAV for each mip (only supported for 1D and 2D textures).
			if ((Desc.Flags & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0 && m_ResourceLocation.GetResource()->CheckUAVSupport() &&
				Desc.DepthOrArraySize == 1)
			{
				m_UAVs.resize(Desc.MipLevels);
				for (int i = 0; i < Desc.MipLevels; ++i)
				{
					auto UavDesc = GetUAVDesc(Desc, i);
					auto Uav = CreateScope<FDX12UnorderedAccessResourceView>(m_ResourceLocation.GetResource(), nullptr, &UavDesc);
					m_UAVs[i] = std::move(Uav);
				}
			}
		}
	}

	void FDX12Texture2D::MakeSRVs(const std::vector<FTextureSubresourceDesc>& Descs)
	{
		bool bCheckSRVSupport = m_ResourceLocation.GetResource()->CheckSRVSupport();
		CD3DX12_RESOURCE_DESC Desc(m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc());
		m_SRVFormat = GetSRVFormat(Desc.Format);

		if (m_SRVs.size() != Descs.size())
		{
			m_SRVs.resize(Descs.size());
			m_SRVDescs.clear();
		}

		
		for (uint32_t i = 0; i < Descs.size(); ++i)
		{
			auto& SubresourceDesc = Descs[i];
			if (m_SRVDescs.size() == i)
			{
				m_SRVDescs.push_back(SubresourceDesc);
			}
			else if (m_SRVDescs[i] == SubresourceDesc)
			{
				continue;
			}

			D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc
			{
				.Format = m_SRVFormat == DXGI_FORMAT_UNKNOWN ?
				m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc().Format : m_SRVFormat,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Texture2D = {
					.MostDetailedMip = SubresourceDesc.FirstMip,
					.MipLevels = SubresourceDesc.MipCount,
					.PlaneSlice = 0,
					.ResourceMinLODClamp = 0.0f,
				},
			};
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
			auto Srv = CreateScope<FDX12ShaderResourceView>(m_ResourceLocation.GetResource(), &SrvDesc);
			m_SRVs[i] = std::move(Srv);
		}
	}

	void FDX12Texture2D::MakeRTVs(const std::vector<FTextureSubresourceDesc>& Descs)
	{
		bool bCheckRTVSupport = m_ResourceLocation.GetResource()->CheckRTVSupport();
		CORE_ASSERT(bCheckRTVSupport, "Check RTVSupport");

		if (m_RTVs.size() != Descs.size())
		{
			m_RTVs.resize(Descs.size());
			m_RTVDescs.clear();
		}

		for (uint32_t i = 0; i < Descs.size();++i)
		{ 
			auto& SubresourceDesc = Descs[i];
			if (m_RTVDescs.size() == i)
			{
				m_RTVDescs.push_back(SubresourceDesc);
			}
			else if (m_RTVDescs[i] == SubresourceDesc)
			{
				continue;
			}
			D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
			switch (m_TextureDesc.Format)
			{ 
			case EResourceFormat::R16_TYPELESS:	
				RTVDesc.Format = DXGI_FORMAT_R16_UNORM;
				break;
			case EResourceFormat::R32_TYPELESS:
				RTVDesc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case EResourceFormat::R24G8_TYPELESS:
				RTVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case EResourceFormat::R32G8X24_TYPELESS:
				RTVDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
				break;
			default:
				RTVDesc.Format = FDX12Utils::ConvertResourceFormat(m_TextureDesc.Format);
				break;
			}
			if (m_TextureDesc.ArraySize > 1)
			{
				if (m_TextureDesc.SampleCount > 1)
				{
					RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
					RTVDesc.Texture2DMSArray.FirstArraySlice = SubresourceDesc.FirstSlice;
					RTVDesc.Texture2DMSArray.ArraySize = SubresourceDesc.SliceCount;
				}
				else
				{
					RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					RTVDesc.Texture2DArray.FirstArraySlice = SubresourceDesc.FirstSlice;
					RTVDesc.Texture2DArray.ArraySize = SubresourceDesc.SliceCount;
					RTVDesc.Texture2DArray.MipSlice = SubresourceDesc.FirstMip;
				}
			}
			else
			{
				if (m_TextureDesc.SampleCount > 1)
				{
					RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
					RTVDesc.Texture2D.MipSlice = SubresourceDesc.FirstMip;
					RTVDesc.Texture2D.PlaneSlice = 0;
				}
			}
			auto Rtv = CreateScope<FDX12RenderTargetView>(m_ResourceLocation.GetResource(), &RTVDesc);
			m_RTVs[i] = std::move(Rtv);
		}
	}

	void FDX12Texture2D::MakeDSVs(const std::vector<FTextureSubresourceDesc>& Descs)
	{
		bool bCheckDSVSupport = m_ResourceLocation.GetResource()->CheckDSVSupport();
		CORE_ASSERT(bCheckDSVSupport, "Check DSVSutrue, pport");

		if (m_DSVs.size() != Descs.size())
		{
			m_DSVs.resize(Descs.size());
			m_DSVDescs.clear();
		}

		for (uint32_t i = 0; i < Descs.size(); ++i)
		{
			auto& SubresourceDesc = Descs[i];
			if (m_DSVDescs.size() == i)
			{
				m_DSVDescs.push_back(SubresourceDesc);
			}
			else if (m_DSVDescs[i] == SubresourceDesc)
			{
				continue;
			}

			D3D12_DEPTH_STENCIL_VIEW_DESC DSVDesc;
			DSVDesc.Flags = D3D12_DSV_FLAG_NONE;
			switch (m_TextureDesc.Format)
			{
			case EResourceFormat::R16_TYPELESS:
				DSVDesc.Format = DXGI_FORMAT_D16_UNORM;
				break;
			case EResourceFormat::R32_TYPELESS:
				DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
				break;
			case EResourceFormat::R24G8_TYPELESS:
				DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				break;
			case EResourceFormat::R32G8X24_TYPELESS:
				DSVDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				break;
			default:
				DSVDesc.Format = FDX12Utils::ConvertResourceFormat(m_TextureDesc.Format);
				break;
			}
			if (m_TextureDesc.ArraySize > 1)
			{
				if (m_TextureDesc.SampleCount > 1)
				{
					DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
					DSVDesc.Texture2DMSArray.FirstArraySlice = SubresourceDesc.FirstSlice;	
					DSVDesc.Texture2DMSArray.ArraySize = SubresourceDesc.SliceCount;
				}
				else
				{
					DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
					DSVDesc.Texture2DArray.FirstArraySlice = SubresourceDesc.FirstSlice;
					DSVDesc.Texture2DArray.ArraySize = SubresourceDesc.SliceCount;
					DSVDesc.Texture2DArray.MipSlice = SubresourceDesc.FirstMip;
				}
			}
			else
			{
				if (m_TextureDesc.SampleCount > 1)
				{
					DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					DSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
					DSVDesc.Texture2D.MipSlice = SubresourceDesc.FirstMip;
				}
			}

			auto Dsv = CreateScope<FDX12DepthStencilView>(m_ResourceLocation.GetResource(), &DSVDesc);
			m_DSVs[i] = std::move(Dsv);
		}
	}

	void FDX12Texture2D::MakeUAVs(const std::vector<FTextureSubresourceDesc>& Descs)
	{
		bool bCheckUAVSupport = m_ResourceLocation.GetResource()->CheckUAVSupport();
		CORE_ASSERT(bCheckUAVSupport, "Check UAVSupport");

		if (m_UAVs.size() != Descs.size())
		{
			m_UAVs.resize(Descs.size());
			m_UAVDescs.clear();
		}

		for (uint32_t i = 0; i < Descs.size(); ++i)
		{ 
			auto& SubresourceDesc = Descs[i];
			if (m_UAVDescs.size() == i)
			{
				m_UAVDescs.push_back(SubresourceDesc);
			}
			else if (m_UAVDescs[i] == SubresourceDesc)
			{
				continue;
			}

			D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
			switch (m_TextureDesc.Format)
			{
			case EResourceFormat::R16_TYPELESS:
				UAVDesc.Format = DXGI_FORMAT_R16_UNORM;
				break;
			case EResourceFormat::R32_TYPELESS:
				UAVDesc.Format = DXGI_FORMAT_D32_FLOAT;
				break;
			case EResourceFormat::R24G8_TYPELESS:
				UAVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case EResourceFormat::R32G8X24_TYPELESS:
				UAVDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
				break;
			default:
				UAVDesc.Format = FDX12Utils::ConvertResourceFormat(m_TextureDesc.Format);
				break;
			}
			if (m_TextureDesc.ArraySize > 1)
			{
				UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				UAVDesc.Texture2DArray.FirstArraySlice = SubresourceDesc.FirstSlice;
				UAVDesc.Texture2DArray.ArraySize = SubresourceDesc.SliceCount;
				UAVDesc.Texture2DArray.MipSlice = SubresourceDesc.FirstMip;
			}
			else
			{
				UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				UAVDesc.Texture2D.MipSlice = SubresourceDesc.FirstMip;
			}

			auto Uav = CreateScope<FDX12UnorderedAccessResourceView>(m_ResourceLocation.GetResource(), nullptr, &UAVDesc);
			m_UAVs[i] = std::move(Uav);
		}
	}

	void FDX12Texture2D::GenerateMip()
	{
		FDX12Device::Get()->GetMipCommandList()->GenerateMipSimple(GetResource());
	}

	void FDX12Texture2D::ReleaseDSVs()
	{
		m_DSVs.clear();
	}

	void FDX12Texture2D::ReleaseUAVs()
	{
		m_UAVs.clear();
	}

	void FDX12Texture2D::ReleaseRTVs()
	{
		m_RTVs.clear();
	}

	void FDX12Texture2D::ReleaseSRVs() 
	{
		m_SRVs.clear();
	}

	FResourceView* FDX12Texture2D::GetRTV(uint32_t ViewID) const
	{
		return m_RTVs[ViewID].get();
	}

	FResourceView* FDX12Texture2D::GetDSV(uint32_t ViewID) const
	{
		return m_DSVs[ViewID].get();
	}

	FResourceView* FDX12Texture2D::GetSRV(uint32_t ViewID) const
	{
		return m_SRVs[ViewID].get();
	}

	FResourceView* FDX12Texture2D::GetUAV(uint32_t mip) const
	{
		return m_UAVs[mip].get();
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
	DXGI_FORMAT FDX12Texture2D::GetSRVFormat(DXGI_FORMAT Format)
	{
		switch (Format)
		{
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		default:
			return DXGI_FORMAT_UNKNOWN;
		}
		return DXGI_FORMAT_UNKNOWN;
	}
}
