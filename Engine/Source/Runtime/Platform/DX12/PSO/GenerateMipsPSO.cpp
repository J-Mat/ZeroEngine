#pragma once

#include "Core.h"
#include "Core/Framework/Library.h"
#include "../Common/DX12Header.h"
#include "GenerateMipsPSO.h"
#include "Platform/DX12/DX12RootSignature.h"
#include "Platform/DX12/DX12Device.h"
#include "../Shader/DX12Shader.h"
#include "Runtime/Render/RHI/Shader/Shader.h"
#include "Render/Moudule/PSOCache.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Platform/DX12/PSO/DX12ComputePipelineStateObject.h"


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
		/*
		FDX12ComputePipelineStateObject* Pso = static_cast<FDX12ComputePipelineStateObject*>(FPSOCache::Get().FetchComputePso(EComputePsoID::PrefixSumTex).get());
		m_D3DPipelineState = Pso->GetD3D12PipelineState();
		m_RootSignature.reset(Pso->GetRootSignature());
		*/
		auto D3DDevice = FDX12Device::Get()->GetDevice();
		CD3DX12_DESCRIPTOR_RANGE1 OutMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
		CD3DX12_ROOT_PARAMETER1 RootParameters[1];
		RootParameters[0].InitAsDescriptorTable(1, &OutMip);
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc(1, RootParameters, 0, nullptr);
		m_RootSignature = CreateRef<FDX12RootSignature>(RootSignatureDesc.Desc_1_1);
		auto GenerateMipShader = TLibrary<FShader>::Fetch("PrefixSum");
		auto* DX12ComputeShader = static_cast<FDX12Shader*>(GenerateMipShader.get());
		D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePsoDesc
		{
			.pRootSignature = m_RootSignature->GetD3D12RootSignature().Get(),
			.CS = {
				reinterpret_cast<BYTE*>(DX12ComputeShader->GetPointer(EShaderStage::CS)),
				DX12ComputeShader->GetLength(EShaderStage::CS)
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateComputePipelineState(&ComputePsoDesc, IID_PPV_ARGS(&m_D3DPipelineState)));
		/*
		auto D3DDevice = FDX12Device::Get()->GetDevice();
		CD3DX12_DESCRIPTOR_RANGE1 SrcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
		CD3DX12_DESCRIPTOR_RANGE1 OutMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
		CD3DX12_ROOT_PARAMETER1 RootParameters[3];
		RootParameters[0].InitAsConstants(sizeof(FGenerateMipsCBTest) / 4, 0);
		RootParameters[1].InitAsDescriptorTable(1, &SrcMip);
		RootParameters[2].InitAsDescriptorTable(1, &OutMip);
		std::vector<EShaderSampler> ShaderSamplers = { EShaderSampler::LinearClamp };
		auto StaticSamplers = FDX12Device::GetStaticSamplers(ShaderSamplers);
		CD3DX12_STATIC_SAMPLER_DESC LinearClampSampler;
		LinearClampSampler.Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP);	
		
		std::vector<CD3DX12_STATIC_SAMPLER_DESC> Samplers = { LinearClampSampler };

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSignatureDesc(3, RootParameters, 1, &LinearClampSampler);

		m_RootSignature = CreateRef<FDX12RootSignature>(RootSignatureDesc.Desc_1_1);

		auto GenerateMipShader = TLibrary<FShader>::Fetch(GENERATE_MIP_SHADER_TEST);
		auto* DX12ComputeShader = static_cast<FDX12Shader*>(GenerateMipShader.get());
		D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePsoDesc
		{
			.pRootSignature = m_RootSignature->GetD3D12RootSignature().Get(),
			.CS = {
				reinterpret_cast<BYTE*>(DX12ComputeShader->GetPointer(EShaderStage::CS)),
				DX12ComputeShader->GetLength(EShaderStage::CS)
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateComputePipelineState(&ComputePsoDesc, IID_PPV_ARGS(&m_D3DPipelineState)));
		*/
	}
}

