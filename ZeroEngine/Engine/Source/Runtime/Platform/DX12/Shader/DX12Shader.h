#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RHI/Shader.h"

namespace Zero
{
	class FDX12Device;
	class FPipelineStateObject;
	class FDX12CommandList;
	class FDX12Shader : public IShader
	{
	public:
		FDX12Shader(std::string FileName, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);

		virtual void CreateBinder();
		virtual void Use() override;
	private:
		void GenerateInputLayout();
		ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);
		void CreatePSO();

	private:

		ComPtr<ID3DBlob> m_VSBytecode = nullptr;
		ComPtr<ID3DBlob> m_PSBytecode = nullptr;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayoutDesc;
		Ref<FPipelineStateObject> m_PipelineStateObject;
	};
}