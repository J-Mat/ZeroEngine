#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RHI/Shader.h"
#include "d3d12shader.h"

namespace Zero
{
	class FDX12Device;
	class FDX12PipelineStateObject;
	class FDX12CommandList;
	class FDX12Shader : public FShader
	{
		friend class FDX12PipelineStateObject;
	public:
		FDX12Shader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		FDX12Shader(const FShaderDesc& Desc);

		virtual void CreateBinder();
		virtual void Use() override;

		
		void GetShaderParameters(ComPtr<ID3DBlob> PassBlob, EShaderType ShaderType);
	private:
		void ParseCBVariable(ID3D12ShaderReflectionVariable* ReflectionVariable, std::vector<FCBVariableItem>& VariableList);
		void GenerateInputLayout();
		ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);

	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayoutDesc;
		std::unordered_map<EShaderType, ComPtr<ID3DBlob>> m_ShaderPass;
	};
}