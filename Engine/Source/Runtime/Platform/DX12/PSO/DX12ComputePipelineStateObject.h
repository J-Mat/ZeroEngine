#pragma once

#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Render/RenderConfig.h"

namespace Zero
{
    class FDX12Device;
    class FDX12ComputePipelineStateObject : public FComputePipelineStateObject
    {
    public:
        FDX12ComputePipelineStateObject(const FComputePSODescriptor& PSODescriptor);
        ~FDX12ComputePipelineStateObject() = default;
        ComPtr<ID3D12PipelineState> GetD3D12PipelineState() const
        {
            return m_D3DPipelineState;
        }
        virtual void Bind(FCommandListHandle CommandListHandle) override;
        virtual void CreateComputePsoObj() override;

        FDX12RootSignature* GetRootSignature() { return m_DX12RootSignature; }

    private:
        ComPtr<ID3D12PipelineState> m_D3DPipelineState;
        FDX12RootSignature* m_DX12RootSignature;
    };
} 
