#pragma once

#include "Core.h"
#include "Common/DX12Header.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/RenderConfig.h"

namespace Zero
{
    class FDX12Device;
    class FDX12PipelineStateObject : public FPipelineStateObject
    {
    public:
        FDX12PipelineStateObject(const FPSODescriptor& PSODescriptor);
        ~FDX12PipelineStateObject() = default;
        ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const
        {
            return m_D3DPipelineState;
        }
        virtual void Bind() override;
        virtual void CreatePsoObj() override;

    private:
        ComPtr<ID3D12PipelineState> m_D3DPipelineState;
    };
} 
