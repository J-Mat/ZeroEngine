#include "DX12PipelineStateObject.h"
#include "../DX12Device.h"
#include "../Shader/DX12ComputeShader.h"
#include "../Shader/DX12ShaderBinder.h"
#include "../DX12CommandList.h"
#include "DX12ComputePipelineStateObject.h"

namespace Zero
{
	FDX12ComputePipelineStateObject::FDX12ComputePipelineStateObject(const FComputePSODescriptor& PSODescriptor)
		: FComputePipelineStateObject(PSODescriptor)
	{
		CreateComputePsoObj();
	}

	void FDX12ComputePipelineStateObject::Bind()
	{
	}

	void FDX12ComputePipelineStateObject::CreateComputePsoObj()
	{
		auto* DX12ComputeShader = static_cast<FDX12ComputeShader*>(m_ComputePSODescriptor.Shader.get());
		FDX12RootSignature* DX12RootSignature = static_cast<FDX12RootSignature*>(DX12ComputeShader->m_ShaderBinder->GetRootSignature());
		D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePsoDesc
		{
			.pRootSignature = DX12RootSignature->GetD3D12RootSignature().Get(),
			.CS = {
				reinterpret_cast<BYTE*>(DX12ComputeShader->m_ShaderPass->GetBufferPointer()),
				DX12ComputeShader->m_ShaderPass->GetBufferSize()
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateComputePipelineState(&ComputePsoDesc, IID_PPV_ARGS(&m_D3DPipelineState)));
	}

}
