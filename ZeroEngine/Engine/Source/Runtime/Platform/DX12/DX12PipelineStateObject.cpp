#include "DX12PipelineStateObject.h"
#include "DX12Device.h"
#include "Shader/DX12Shader.h"
#include "Shader/DX12ShaderBinder.h"
#include "DX12CommandList.h"

namespace Zero
{
	FDX12PipelineStateObject::FDX12PipelineStateObject(const FPSODescriptor& PSODescriptor)
		:FPipelineStateObject(PSODescriptor)
	{
		auto* D3DShader =  static_cast<FDX12Shader*>(PSODescriptor.Shader.get());
		FDX12ShaderBinder* DX12ShaderBinder = static_cast<FDX12ShaderBinder*>(D3DShader->m_ShaderBinder.get());
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		PsoDesc.InputLayout = { D3DShader->m_InputLayoutDesc.data(), (UINT)D3DShader->m_InputLayoutDesc.size() };
		FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(DX12ShaderBinder->GetRootSignature());
		PsoDesc.pRootSignature = D3DRootSignature->GetD3D12RootSignature().Get();
		PsoDesc.VS = { reinterpret_cast<BYTE*>(D3DShader->m_VSBytecode->GetBufferPointer()), D3DShader->m_VSBytecode->GetBufferSize() };
		PsoDesc.PS = { reinterpret_cast<BYTE*>(D3DShader->m_PSBytecode->GetBufferPointer()), D3DShader->m_PSBytecode->GetBufferSize() };
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState.DepthFunc = FDX12RHItConverter::GetComparisonFunc(PSODescriptor.DepthFunc);
		PsoDesc.SampleMask = UINT_MAX;	//0xffffffff, No Sampling Mask
		PsoDesc.PrimitiveTopologyType = FDX12RHItConverter::GetTopologyType(PSODescriptor.TopologyType);
		PsoDesc.NumRenderTargets = D3DShader->m_ShaderDesc.NumRenderTarget;
		PsoDesc.RasterizerState.CullMode = FDX12RHItConverter::GetCullMode(PSODescriptor.CullMode);
		for (int i = 0; i < D3DShader->m_ShaderDesc.NumRenderTarget; i++)
			PsoDesc.RTVFormats[i] = FDX12Texture2D::GetFormatByDesc(D3DShader->m_ShaderDesc.Formats[i]);
		PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		PsoDesc.SampleDesc.Count = 1;	// No 4XMSAA
		PsoDesc.SampleDesc.Quality = PSODescriptor._4xMsaaQuality;	////No 4XMSAA
		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_D3DPipelineState)));
	}

	void FDX12PipelineStateObject::Bind()
	{
		Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetRenderCommandList();
		CommandList->GetD3D12CommandList()->SetPipelineState(m_D3DPipelineState.Get());
	}
}
