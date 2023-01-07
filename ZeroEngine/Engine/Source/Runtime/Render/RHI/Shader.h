#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "VertexBuffer.h"
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
		int NumRenderTarget = 2;
		FFrameBufferTexturesFormats Formats = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::DEPTH32F
		};

		FShaderDefines ShaderDefines;
		bool bCreateVS = true;
		std::string VSEntryPoint = "VS";
		bool bCreatePS = true;
		std::string PSEntryPoint = "PS";
		bool bCreateCS = false;
		std::string CSEntryPoint = "CS";
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
		virtual void Use() = 0;
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
	
}