#include "PipelineStateObject.h"
#include "DX12Device.h"

namespace Zero
{
	FPipelineStateObject::FPipelineStateObject(FDX12Device& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc)
		:m_Device(Device)
	{
		ThrowIfFailed(m_Device.GetDevice()->CreateGraphicsPipelineState(&Desc, IID_PPV_ARGS(&m_D3DPipelineState)));
	}
}
