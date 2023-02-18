#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "./Buffer/VertexBuffer.h"
#include "Texture.h"

namespace Zero
{
	struct FPipelineStateDesc;

	class FShaderDefines
	{	
	public:
		void SetDefine(const std::string& Name, const std::string& Definition);
		bool operator == (const FShaderDefines& Other) const;
	private:
		std::unordered_map<std::string, std::string> DefinesMap;
	};

	struct FShaderDesc
	{
		std::string ShaderName;
		bool bUseAlphaBlending = false;
		FVertexBufferLayout VertexBufferLayout = FVertexBufferLayout::s_DefaultVertexLayout;
		int32_t NumRenderTarget = 1;
		FFrameBufferTexturesFormats Formats = {
			EResourceFormat::R8G8B8A8_UNORM,
		};
		
		bool bNeedDetph = true;
		EResourceFormat DepthFormat = EResourceFormat::D24_UNORM_S8_UINT;
		

		FShaderDefines ShaderDefines;
		bool bCreateVS = true;
		std::string VSEntryPoint = "VS";
		bool bCreatePS = true;
		std::string PSEntryPoint = "PS";
	};
		
	struct FShaderSamplerParameter : FShaderParameter
	{
	};

	class FShader
	{
	public:
		FShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		FShader(const FShaderDesc& Desc);
		virtual ~FShader() {}
		virtual void Use(FCommandListHandle CommandList) = 0;
		virtual void CreateBinder() = 0;

		virtual Ref<IShaderBinder> GetBinder() { return m_ShaderBinder; }

		const std::map<std::pair<uint32_t, uint32_t>, FShaderCBVParameter>& GetCBVParams() const { return m_CBVParams; }
		const std::map<std::pair<uint32_t, uint32_t>, FShaderSRVParameter>& GetSRVParams() const { return m_SRVParams; }
		
		void GenerateShaderDesc();

		const FShaderDesc& GetDesc() { return m_ShaderDesc; }

	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;

		std::map<std::pair<uint32_t, uint32_t>, FShaderCBVParameter> m_CBVParams;
		std::map<std::pair<uint32_t, uint32_t>, FShaderSRVParameter> m_SRVParams;
		std::vector<FShaderUAVParameter> m_UAVParams;
		std::vector<FShaderSamplerParameter> m_SamplerParams;
	};

	struct FComputeShaderDesc
	{
		std::string ShaderName;
		FShaderDefines ShaderDefines;
		std::string CSEntryPoint = "CS";
		uint32_t BlockSize_X = 8;
		uint32_t BlockSize_Y = 8;
		uint32_t BlockSize_Z = 8;
	};

	class FComputeShader
	{
	public:
		FComputeShader(const FShaderBinderDesc& BinderDesc, const FComputeShaderDesc& Desc);
		FComputeShader(const FComputeShaderDesc& Desc);
	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FComputeShaderDesc m_ComputeShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;
	};
	
}