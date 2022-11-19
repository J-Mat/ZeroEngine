#include "PipelineStateObject.h"
#include "DX12Device.h"

namespace Zero
{
	FPipelineStateObject::FPipelineStateObject(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc)
	{
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateGraphicsPipelineState(&Desc, IID_PPV_ARGS(&m_D3DPipelineState)));
	}
}
