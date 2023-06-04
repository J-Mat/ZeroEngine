#include "DX12Buffer.h" 
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12Buffer::FDX12Buffer(const FBufferDesc& Desc, void* Data /*= 0*/, bool bCreateTextureView /*= true*/)
		: FBuffer(Desc, Data)
	{
		uint64_t BufferSize = m_Desc.Size;
		if (HasAnyFlag(m_Desc.BufferMiscFlag, EBufferMiscFlag::ConstantBuffer))
			BufferSize = ZMath::AlignUp(BufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

		D3D12_RESOURCE_DESC ResourceDesc
		{
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = 0,
			.Width = BufferSize,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc =
			{
				.Count = 1,
				.Quality = 0
			},
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};
		if (HasAllFlags(m_Desc.ResourceBindFlag, EResourceBindFlag::UnorderedAccess))
			ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		if (!HasAllFlags(m_Desc.ResourceBindFlag, EResourceBindFlag::ShaderResource))
			ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;
		if (HasAllFlags(m_Desc.BufferMiscFlag, EBufferMiscFlag::AccelStruct))
			ResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		
		if (m_Desc.ResourceUsage == EResourceUsage::Upload)
		{
			auto UploadBufferAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
			UploadBufferAllocator->AllocUploadResource(m_Desc.Size, UPLOAD_RESOURCE_ALIGNMENT, m_ResourceLocation);
		}

		if (Data != nullptr)
		{
			FDX12Device::Get()->GetInitWorldCommandList()->CreateAndInitDefaultBuffer(
				Data,
				m_Desc.Size,
				DEFAULT_RESOURCE_ALIGNMENT,
				m_ResourceLocation);
			/*
			m_D3DResource = FDX12Device::Get()->GetInitWorldCommandList()->CreateDefaultBuffer(
				m_Data,
				m_Desc.Size
			);
			*/
		}
		if (bCreateTextureView)
		{
			CreateViews();
		}
	}



	void FDX12Buffer::Update(Ref<FCommandList> CommandList, void const* SrcData, size_t DataSize, size_t Offset /*= 0*/)
	{
		FResourceLocation UploadResourceLocation;
		auto UploadBufferAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
		BYTE* MappedData = (BYTE*)UploadBufferAllocator->AllocUploadResource(DataSize, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);

		memcpy(MappedData + Offset, SrcData, DataSize);
		
		Ref<FDX12Resource> UnderlyingResource = m_ResourceLocation.m_UnderlyingResource;
		Ref<FDX12Resource> UploadBuffer = UploadResourceLocation.m_UnderlyingResource;
		
		FDX12CommandList* DX12CommandList = static_cast<FDX12CommandList*>(CommandList.get());
		DX12CommandList->TransitionBarrier(UnderlyingResource->GetD3DResource().Get(), D3D12_RESOURCE_STATE_COPY_DEST);
		DX12CommandList->CopyBufferRegion(UnderlyingResource->GetD3DResource(), m_ResourceLocation.m_OffsetFromBaseOfResource,
			UploadBuffer->GetD3DResource(), UploadResourceLocation.m_OffsetFromBaseOfResource, DataSize);
	}


	D3D12_GPU_VIRTUAL_ADDRESS FDX12Buffer::GetGPUAddress()
	{
		return m_ResourceLocation.m_GPUVirtualAddress; 
		//return m_D3DResource->GetGPUVirtualAddress();
	}

	void FDX12Buffer::CreateViews()
	{
		if (m_ResourceLocation.GetResource()->GetD3DResource())
		{
			auto D3DDevice = FDX12Device::Get()->GetDevice();
			
			CD3DX12_RESOURCE_DESC Desc(m_ResourceLocation.GetResource()->GetD3DResource()->GetDesc());

			// Create SRV
			m_SRVFormat = FDX12Utils::GetSRVFormat(Desc.Format);
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

	Zero::FResourceView* FDX12Buffer::GetSRV()
	{

	}

	Zero::FResourceView* FDX12Buffer::GetUAV()
	{

	}

	void FDX12Buffer::MakeSRVs(const std::vector<FBufferSubresourceDesc>& Descs)
	{

	}

	void FDX12Buffer::MakeUAVs(const std::vector<FBufferSubresourceDesc>& Descs)
	{

	}

	void FDX12Buffer::ReleaseSRVs()
	{

	}

	void FDX12Buffer::ReleaseUAVs()
	{

	}

}

