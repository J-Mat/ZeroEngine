#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/Shader.h"
#include "d3d12shader.h"

namespace Zero
{
	class FDX12Device;
	class FDX12ComputePipelineStateObject;
	class FDX12CommandList;
	class FDX12ComputePipelineStateObject;
	class FDX12ComputeShader : public FComputeShader
	{
		friend class FDX12ComputePipelineStateObject;
	public:
		FDX12ComputeShader(const FShaderBinderDesc& BinderDesc, const FComputeShaderDesc& Desc);
		FDX12ComputeShader(const FComputeShaderDesc& Desc);
		virtual void Dispatch(uint32_t X, uint32_t Y, uint32_t Z) override;
		virtual void CreateBinder();
		ComPtr<ID3DBlob> GetShaderPass() { return m_ShaderPass; }
	private:
		ComPtr<ID3DBlob> m_ShaderPass;
		ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);
	};
}
