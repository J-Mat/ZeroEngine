#pragma once

#include "Core.h"
#include "Common/DX12Header.h"

namespace Zero
{
    class FDX12Device;
    class FPipelineStateObject
    {
    public:
        FPipelineStateObject(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc);
        ~FPipelineStateObject() = default;
        ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const
        {
            return m_D3DPipelineState;
        }

    private:
        ComPtr<ID3D12PipelineState> m_D3DPipelineState;
    };
}  // 
