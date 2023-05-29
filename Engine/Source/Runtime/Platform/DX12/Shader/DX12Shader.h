#pragma once
#include "Core.h"
#include "../Common/DX12Header.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RHI/Shader/Shader.h"
#include "d3d12shader.h"

namespace Zero
{
	class FDX12Device;
	class FDX12GraphicPipelineStateObject;
	class FDX12ComputePipelineStateObject;
	class FDX12CommandList;
	class FDX12Shader : public FShader
	{
		friend class FDX12GraphicPipelineStateObject;
		friend class FDX12ComputePipelineStateObject;
	public:
		FDX12Shader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		FDX12Shader(const FShaderDesc& Desc);

		virtual void CreateBinder();
		virtual void Use(FCommandListHandle CommandList) override;

		
		void GetShaderParameters(ComPtr<ID3DBlob> PassBlob, EShaderStage ShaderStage);
		void ParseShader(ID3D12ShaderReflection* ShaderReflectionPtr, EShaderStage ShaderStage);

		D3D12_SHADER_BYTECODE GetD3DShaderBytecode(EShaderStage Stage) const
		{
			D3D12_SHADER_BYTECODE Bytecode{};
			Bytecode.pShaderBytecode = GetPointer(Stage);
			Bytecode.BytecodeLength = GetLength(Stage);
			return Bytecode;
		}

	protected:
		void ParseCBVariable(ID3D12ShaderReflectionVariable* ReflectionVariable, std::vector<FCBVariableItem>& VariableList);
		void GenerateInputLayout();

	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayoutDesc;
		//std::unordered_map<EShaderStage, ComPtr<ID3DBlob>> m_ShaderPass;
	};

	class FDX12GraphicShader : public FDX12Shader
	{
	public:
		FDX12GraphicShader(const FShaderDesc& Desc);
		virtual void Compile() override;
	};

	class FDX12ComputeShader : public FDX12Shader
	{
	public:
		FDX12ComputeShader(const FShaderDesc& Desc);
		virtual void Compile() override;
	};
}