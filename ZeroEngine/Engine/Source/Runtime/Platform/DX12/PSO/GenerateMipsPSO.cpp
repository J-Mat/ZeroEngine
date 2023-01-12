#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "GenerateMipsPSO.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Platform/DX12/DX12Device.h"
#include "Runtime/Render/RHI/Shader.h"


namespace Zero
{
	struct alignas(16) GenerateMipsCB
	{
		uint32_t SrcMipLevel; // Texture level of source mip
		uint32_t NumMipLevels;  // Number of OutMips to write: [1-4]
		uint32_t SrcDimension;  // Width and height of the source texture are even or odd.
	};

	FGenerateMipsPSO::FGenerateMipsPSO()
	{
		auto D3DDevice = FDX12Device::Get()->GetDevice();

		CD3DX12_DESCRIPTOR_RANGE SrcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
		CD3DX12_DESCRIPTOR_RANGE OutMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

		std::vector<CD3DX12_ROOT_PARAMETER> RootParameters(EGenerateMips::GM_NumRootParameters);
		RootParameters[EGenerateMips::GM_GenerateMipsCB].InitAsConstants(sizeof(FGenerateMipsCB) / 4, 0);
		RootParameters[EGenerateMips::GM_SrcMip].InitAsDescriptorTable(1, &SrcMip);
		RootParameters[EGenerateMips::GM_OutMip].InitAsDescriptorTable(1, &OutMip);

		CD3DX12_STATIC_SAMPLER_DESC LinearClampSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	
		std::vector<CD3DX12_STATIC_SAMPLER_DESC> Samplers = { LinearClampSampler };

		m_RootSignature = CreateRef<FDX12RootSignature>(RootParameters, Samplers);

		FShaderDesc ShaderDesc
		{
			.ShaderName = "GenerateMip.hlsl",
			.bUseAlphaBlending = false,
			.NumRenderTarget = 0,
			.Formats = {},
			.bCreateVS = false,
			.bCreatePS = false,
			.bCreateCS = true
		};
	}
}

