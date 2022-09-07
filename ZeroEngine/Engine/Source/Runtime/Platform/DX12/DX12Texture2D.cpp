#include "DX12Texture2D.h"
#include "DX12Device.h"
#include "DX12CommandQueue.h"
#include "DX12CommandList.h"


namespace Zero
{
	FDX12Texture2D::FDX12Texture2D(FDX12Device& Device, const D3D12_RESOURCE_DESC& ResourceDesc, const D3D12_CLEAR_VALUE* clearValue)
		: IResource(Device, ResourceDesc, clearValue)
		, m_Device(Device)
	{
		CreateViews();
	}

	FDX12Texture2D::FDX12Texture2D(FDX12Device& Device, Ref<FImage> ImageData)
		: IResource(Device)
		, m_Device(Device)
	{
		auto Resource = m_Device.GetCommandQueue().GetCommandList()->CreateTextureResource(ImageData);
		SetResource(Resource);
		CreateViews();
	}

	FDX12Texture2D::FDX12Texture2D(FDX12Device& Device, ComPtr<ID3D12Resource> Resource, const D3D12_CLEAR_VALUE* ClearValue)
		: IResource(Device, Resource, ClearValue)
		, m_Device(Device)
	{
		CreateViews();
	}

	void FDX12Texture2D::Resize(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize = 1)
	{
		if (m_D3DResource)
		{
			CD3DX12_RESOURCE_DESC ResDesc(m_D3DResource->GetDesc());
			
			ResDesc.Width = std::max(Width, 1u);
			ResDesc.Height = std::max(Height, 1u);
			ResDesc.DepthOrArraySize = DepthOrArraySize;
			ResDesc.MipLevels = ResDesc.SampleDesc.Count > 1 ? 1 : 0;
			
			ThrowIfFailed(
				m_Device.GetDevice()->CreateCommittedResource(
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
	D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& ResDesc, UINT MipSlice, UINT ArraySlice = 0,
		UINT PlaneSlice = 0)
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
			auto D3DDevice = m_Device.GetDevice();
			
			CD3DX12_RESOURCE_DESC Desc(m_D3DResource->GetDesc());

			// Create RTV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 && CheckRTVSupport())
			{
				m_RenderTargetView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
				D3DDevice->CreateRenderTargetView(m_D3DResource.Get(), nullptr, m_RenderTargetView.GetDescriptorHandle());
			}

			// Create DSV	
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 && CheckDSVSupport())
			{
				m_DepthStencilView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
				D3DDevice->CreateDepthStencilView(m_D3DResource.Get(), nullptr,
					m_DepthStencilView.GetDescriptorHandle());
			}
			// Create SRV
			if ((Desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
			{
				m_ShaderResourceView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				D3DDevice->CreateShaderResourceView(m_D3DResource.Get(), nullptr,
					m_ShaderResourceView.GetDescriptorHandle());
			}
			// Create UAV for each mip (only supported for 1D and 2D textures).
			if ((Desc.Flags & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0 && CheckUAVSupport() &&
				Desc.DepthOrArraySize == 1)
			{
				m_UnorderedAccessView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, Desc.MipLevels);
				for (int i = 0; i < Desc.MipLevels; ++i)
				{
					auto uavDesc = GetUAVDesc(Desc, i);
					D3DDevice->CreateUnorderedAccessView(m_D3DResource.Get(), nullptr, &uavDesc,
						m_UnorderedAccessView.GetDescriptorHandle(i));
				}
			}
		}
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetRenderTargetView() const
	{
		return m_RenderTargetView.GetDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetDepthStencilView() const
	{
		return m_DepthStencilView.GetDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetShaderResourceView() const
	{
		return m_ShaderResourceView.GetDescriptorHandle();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE FDX12Texture2D::GetUnorderedAccessView(uint32_t mip) const
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE();
	}
}
