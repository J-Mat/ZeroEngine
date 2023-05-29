#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
#include "Render/RenderConfig.h"

namespace Zero
{
    class FDX12Device;
    class FDX12GraphicPipelineStateObject : public FGraphicPipelineStateObject
    {
    public:
        FDX12GraphicPipelineStateObject(const FGraphicPSODescriptor& PSODescriptor);
        ~FDX12GraphicPipelineStateObject() = default;
        ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const
        {
            return m_D3DPipelineState;
        }
        virtual void Bind(FCommandListHandle CommandListHandle) override;
        virtual void CreateGraphicPSOObj() override;

    private:
        ComPtr<ID3D12PipelineState> m_D3DPipelineState;
    };
} 
