#pragma once

#include "Core.h"
#include "Common/DX12Header.h"

namespace Zero
{
    class FDX12Device;
    class FPipelineStateObject
    {
    public:
        ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const
        {
            return m_D3DPipelineState;
        }

    protected:
        FPipelineStateObject(FDX12Device& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc);
        virtual ~FPipelineStateObject() = default;

    private:
        FDX12Device& m_Device;
        ComPtr<ID3D12PipelineState> m_D3DPipelineState;
    };
}  // 
