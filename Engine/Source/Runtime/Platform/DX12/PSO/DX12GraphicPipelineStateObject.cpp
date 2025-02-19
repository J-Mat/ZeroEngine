#include "DX12GraphicPipelineStateObject.h"
#include "../DX12Device.h"
#include "../Shader/DX12Shader.h"
#include "../Shader/DX12ShaderBinder.h"
#include "../DX12CommandList.h"

namespace Zero
{
	FDX12GraphicPipelineStateObject::FDX12GraphicPipelineStateObject(const FGraphicPSODescriptor& PSODescriptor)
		:FGraphicPipelineStateObject(PSODescriptor)
	{
		CreateGraphicPSOObj();
	}

	void FDX12GraphicPipelineStateObject::Bind(FCommandListHandle CommandListHandle)
	{
		Ref<FDX12CommandList> CommandList = FDX12Device::Get()->GetCommandList(CommandListHandle);
		CommandList->GetD3D12CommandList()->SetPipelineState(m_D3DPipelineState.Get());
	}

	void FDX12GraphicPipelineStateObject::CreateGraphicPSOObj()
	{
		auto* D3DShader = static_cast<FDX12Shader*>(m_PSODescriptor.Shader.get());
		FDX12ShaderBinder* DX12ShaderBinder = static_cast<FDX12ShaderBinder*>(D3DShader->m_ShaderBinder.get());
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		PsoDesc.InputLayout = { D3DShader->m_InputLayoutDesc.data(), (UINT)D3DShader->m_InputLayoutDesc.size() };
		FDX12RootSignature* D3DRootSignature = static_cast<FDX12RootSignature*>(DX12ShaderBinder->GetRootSignature());
		PsoDesc.pRootSignature = D3DRootSignature->GetD3D12RootSignature().Get();
		PsoDesc.VS = D3DShader->GetD3DShaderBytecode(EShaderStage::VS);  
		PsoDesc.PS = D3DShader->GetD3DShaderBytecode(EShaderStage::PS); 
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.DepthStencilState.DepthFunc = FDX12Utils::GetComparisonFunc(m_PSODescriptor.DepthFunc);
		PsoDesc.DepthStencilState.DepthEnable = m_PSODescriptor.bDepthEnable;
		PsoDesc.SampleMask = UINT_MAX;	//0xffffffff, No Sampling Mask
		PsoDesc.PrimitiveTopologyType = FDX12Utils::GetTopologyType(m_PSODescriptor.TopologyType);
		PsoDesc.NumRenderTargets = D3DShader->m_ShaderDesc.NumRenderTarget;
		PsoDesc.RasterizerState.CullMode = FDX12Utils::GetCullMode(m_PSODescriptor.CullMode);
		PsoDesc.RasterizerState.FrontCounterClockwise = false;
		for (int i = 0; i < D3DShader->m_ShaderDesc.NumRenderTarget; i++)
			PsoDesc.RTVFormats[i] = FDX12Utils::ConvertResourceFormat(D3DShader->m_ShaderDesc.Formats[i]);
		if (D3DShader->m_ShaderDesc.bNeedDetph)
		{
			PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		}
		PsoDesc.SampleDesc.Count = 1;	// No 4XMSAA
		PsoDesc.SampleDesc.Quality = m_PSODescriptor._4xMsaaQuality;	////No 4XMSAA

		switch (m_PSODescriptor.PSOType)
		{
		case EPSOType::PT_Normal:
			break;
		case EPSOType::PT_Depth:
			PsoDesc.RasterizerState.DepthBias = 100000;
			PsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
			PsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		default:
			break;
		}

		ThrowIfFailed(FDX12Device::Get()->GetDevice()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_D3DPipelineState)));
	}
}
