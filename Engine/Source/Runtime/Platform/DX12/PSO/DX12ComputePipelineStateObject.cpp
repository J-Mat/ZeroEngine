#include "DX12GraphicPipelineStateObject.h"
#include "../DX12Device.h"
#include "../Shader/DX12Shader.h"
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


	void FDX12ComputePipelineStateObject::Bind(FCommandListHandle CommandListHandle)
	{
		Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		CommandList->SetPipelineState(m_D3DPipelineState.Get());
	}

	void FDX12ComputePipelineStateObject::CreateComputePsoObj()
	{
		auto* DX12ComputeShader = static_cast<FDX12Shader*>(m_ComputePSODescriptor.Shader.get());
		m_DX12RootSignature = static_cast<FDX12RootSignature*>(DX12ComputeShader->m_ShaderBinder->GetRootSignature());
		D3D12_COMPUTE_PIPELINE_STATE_DESC ComputePsoDesc
		{
			.pRootSignature = m_DX12RootSignature->GetD3D12RootSignature().Get(),
			.CS = {
				reinterpret_cast<BYTE*>(DX12ComputeShader->GetPointer(EShaderStage::CS)),
				DX12ComputeShader->GetLength(EShaderStage::CS)
			},
			.Flags = D3D12_PIPELINE_STATE_FLAG_NONE
		};
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateComputePipelineState(&ComputePsoDesc, IID_PPV_ARGS(&m_D3DPipelineState)));
	}

}
