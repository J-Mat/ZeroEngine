#include "DX12Buffer.h" 
#include "DX12Device.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12Buffer::FDX12Buffer(const FBufferDesc& Desc, void* Data)
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
		else if (Data != nullptr)
		{
			FDX12Device::Get()->GetInitWorldCommandList()->CreateAndInitDefaultBuffer(
				m_Data,
				m_Desc.Size,
				DEFAULT_RESOURCE_ALIGNMENT,
				m_ResourceLocation);
		}

	}
}

