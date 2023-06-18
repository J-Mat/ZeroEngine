#include "DX12Buffer.h" 
#include "DX12Device.h"
#include "DX12CommandList.h"


namespace Zero
{
	FDX12Buffer::FDX12Buffer(const FBufferDesc& Desc, void* BufferData /*= 0*/, bool bCreateTextureView /*= true*/)
		: FBuffer(Desc, BufferData)
	{
		uint64_t BufferSize = m_Desc.Size;
		if (HasAnyFlag(m_Desc.MiscFlag, EBufferMiscFlag::ConstantBuffer))
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
		{
			ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		if (!HasAllFlags(m_Desc.ResourceBindFlag, EResourceBindFlag::ShaderResource))
			ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		D3D12_RESOURCE_STATES ResourceState = D3D12_RESOURCE_STATE_COMMON;
		if (HasAllFlags(m_Desc.MiscFlag, EBufferMiscFlag::AccelStruct))
			ResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		
		switch (m_Desc.ResourceUsage)
		{
		case EResourceUsage::Default:
		{
			if (HasAllFlags(m_Desc.ResourceBindFlag, EResourceBindFlag::UnorderedAccess))
			{	
				ID3D12Device* DX12Device = FDX12Device::Get()->GetDevice();
				ThrowIfFailed(DX12Device->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&CD3DX12_RESOURCE_DESC::Buffer(BufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
					D3D12_RESOURCE_STATE_COMMON,
					nullptr,
					IID_PPV_ARGS(&m_D3DResource)));
			m_ResourceLocation.m_UnderlyingResource = CreateRef<FDX12Resource>("Uav_test", m_D3DResource);
			std::cout << "Buffer Ptr: " << m_D3DResource.Get() << std::endl;
			}
			else
			{
				FDX12Device::Get()->GetInitWorldCommandList()->CreateAndInitDefaultBuffer(
					ResourceDesc,
					BufferData,
					m_Desc.Size,
					DEFAULT_RESOURCE_ALIGNMENT,
					m_ResourceLocation);
			}
			break;
		}
		case EResourceUsage::Readback:
		{
			/*
			auto ReadBackBufferAllocator = FDX12Device::Get()->GetReadBackBufferAllocator();
			ReadBackBufferAllocator->AllocReadBackResource(m_Desc.Size, DEFAULT_RESOURCE_ALIGNMENT, m_ResourceLocation);
			break;
			*/
			ID3D12Device* DX12Device = FDX12Device::Get()->GetDevice();
			ThrowIfFailed(DX12Device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(BufferSize),
				D3D12_RESOURCE_STATE_COMMON,
				nullptr,
				IID_PPV_ARGS(&m_D3DResource)));
			m_ResourceLocation.m_UnderlyingResource = CreateRef<FDX12Resource>("Readback_test", m_D3DResource);
		}
		case EResourceUsage::Upload:
		{
			auto UploadBufferAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
			void* UploadBufferMap = UploadBufferAllocator->AllocUploadResource(m_Desc.Size, UPLOAD_RESOURCE_ALIGNMENT, m_ResourceLocation);
			if (BufferData != nullptr)
			{
				memcpy(UploadBufferMap, BufferData, m_Desc.Size);
			}
			break;
		}
		default:
			break;
		}

		if (bCreateTextureView)
		{
			//CreateViews();
		}
	}


		void* FDX12Buffer::Map() const
		{
			if (m_Desc.ResourceUsage == EResourceUsage::Readback)
			{
				float* mappedData = nullptr;
				m_ResourceLocation.m_UnderlyingResource->GetD3DResource()->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
				for (size_t i = 0; i < 32; ++i)
				{
					std::cout << mappedData[i] << " ";
				}
			}
			return  m_ResourceLocation.m_MappedAddress;
		}

		void FDX12Buffer::Update(FCommandListHandle CommandListHandle, void const* SrcData, size_t DataSize, size_t Offset /*= 0*/)
	{
		if (m_Desc.ResourceUsage == EResourceUsage::Default)
		{
			FResourceLocation UploadResourceLocation;
			auto UploadBufferAllocator = FDX12Device::Get()->GetUploadBufferAllocator();
			void* MappedData = UploadBufferAllocator->AllocUploadResource(DataSize, UPLOAD_RESOURCE_ALIGNMENT, UploadResourceLocation);

			memcpy(MappedData, SrcData, DataSize);

			auto DX12CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
			Ref<FDX12Resource>	DefaultBuffer = m_ResourceLocation.m_UnderlyingResource;
			DX12CommandList->TransitionBarrier(DefaultBuffer->GetD3DResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
			return;
			Ref<FDX12Resource>	UploadBuffer = UploadResourceLocation.m_UnderlyingResource;


			DX12CommandList->TransitionBarrier(DefaultBuffer->GetD3DResource().Get(), D3D12_RESOURCE_STATE_COPY_DEST);
			DX12CommandList->CopyBufferRegion(DefaultBuffer->GetD3DResource(), m_ResourceLocation.m_OffsetFromBaseOfResource,
				UploadBuffer->GetD3DResource(), UploadResourceLocation.m_OffsetFromBaseOfResource, DataSize);
			//UploadResourceLocation.ReleaseResource();
			DX12CommandList->TransitionBarrier(DefaultBuffer->GetD3DResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ);
		}
		else
		{
			memcpy((uint8_t*)m_ResourceLocation.GetMapAddress() + Offset, SrcData, DataSize);
		}
	}



	D3D12_GPU_VIRTUAL_ADDRESS FDX12Buffer::GetGPUAddress()
	{
		return m_ResourceLocation.m_GPUVirtualAddress; 
		//return m_D3DResource->GetGPUVirtualAddress();
	}

	void FDX12Buffer::CreateViews()
	{
	}

	FResourceView* FDX12Buffer::GetSRV()
	{
		return m_Srv.get();
	}

	FResourceView* FDX12Buffer::GetUAV()
	{
		return m_Uav.get();
	}

	void FDX12Buffer::CreateSRV(const FBufferSubresourceDesc& SubDesc)
	{
		if (SubDesc == m_SrvSubresourceDesc && m_Srv != nullptr)
		{
			return;
		}
		D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc{};
		SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;

		if (m_Desc.Format == EResourceFormat::UNKNOWN)
		{
			if (HasAllFlags(m_Desc.MiscFlag, EBufferMiscFlag::BufferRaw))
			{
				SrvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				SrvDesc.Buffer.FirstElement = (UINT)SubDesc.Offset / sizeof(uint32_t);
				SrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
				SrvDesc.Buffer.NumElements = (UINT)std::min<UINT64>(SubDesc.Size, m_Desc.Size - SubDesc.Offset) / sizeof(uint32_t);
			}
			else if (HasAllFlags(m_Desc.MiscFlag, EBufferMiscFlag::BufferStructured))
			{
				SrvDesc.Format = DXGI_FORMAT_UNKNOWN;
				SrvDesc.Buffer.FirstElement = (UINT)SubDesc.Offset / m_Desc.Stride;
				SrvDesc.Buffer.NumElements = (UINT)std::min<UINT64>(SubDesc.Size, m_Desc.Size - SubDesc.Offset) / m_Desc.Stride;
				SrvDesc.Buffer.StructureByteStride = m_Desc.Stride;
			}
		}
		else
		{ 
			uint32_t Stride = FDX12Utils::GetFormatStride(m_Desc.Format);
			SrvDesc.Format = FDX12Utils::ConvertResourceFormat(m_Desc.Format);
			SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			SrvDesc.Buffer.FirstElement = SubDesc.Offset / Stride;
			SrvDesc.Buffer.NumElements = (UINT)std::min<UINT64>(SubDesc.Size, m_Desc.Size - SubDesc.Offset) / Stride;
		}
		m_Srv = CreateScope<FDX12ShaderResourceView>(m_ResourceLocation.GetResource(), &SrvDesc);
		m_SrvSubresourceDesc = SubDesc;
	}

	void FDX12Buffer::CreateUAV(const FBufferSubresourceDesc& SubDesc)
	{
		if (SubDesc == m_UavSubresourceDesc && m_Uav != nullptr)
		{
			return;
		}
		D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc{};
		UavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UavDesc.Buffer.FirstElement = 0;

		if (m_Desc.Format == EResourceFormat::UNKNOWN)
		{
			if (HasAllFlags(m_Desc.MiscFlag, EBufferMiscFlag::BufferRaw))
			{
				UavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
				UavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
				UavDesc.Buffer.FirstElement = (UINT)SubDesc.Offset / sizeof(uint32_t);
				UavDesc.Buffer.NumElements = (UINT)std::min<UINT64>(SubDesc.Size, m_Desc.Size - SubDesc.Offset) / sizeof(uint32_t);
			}
			else if (HasAllFlags(m_Desc.MiscFlag, EBufferMiscFlag::BufferStructured))
			{
				UavDesc.Format = DXGI_FORMAT_UNKNOWN;
				UavDesc.Buffer.FirstElement = (UINT)SubDesc.Offset / m_Desc.Stride;
				UavDesc.Buffer.NumElements = (UINT)std::min<UINT64>(SubDesc.Size, m_Desc.Size - SubDesc.Offset) / m_Desc.Stride;
				UavDesc.Buffer.StructureByteStride = m_Desc.Stride;
			}
		}
		else
		{ 
			uint32_t Stride = FDX12Utils::GetFormatStride(m_Desc.Format);
			UavDesc.Format = FDX12Utils::ConvertResourceFormat(m_Desc.Format);
			UavDesc.Buffer.FirstElement = SubDesc.Offset / Stride;
			UavDesc.Buffer.NumElements = (UINT)std::min<UINT64>(SubDesc.Size, m_Desc.Size - SubDesc.Offset) / Stride;
			if (HasAllFlags(m_Desc.MiscFlag, EBufferMiscFlag::BufferRaw))
			{
				UavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
			}
		}
		m_Uav = CreateScope<FDX12UnorderedAccessResourceView>(m_ResourceLocation.GetResource(), nullptr, &UavDesc);
		m_UavSubresourceDesc = SubDesc;

	}

	void FDX12Buffer::ReleaseSRVs()
	{
		m_Srv = nullptr;
	}

	void FDX12Buffer::ReleaseUAVs()
	{
		m_Uav = nullptr;
	}

}

