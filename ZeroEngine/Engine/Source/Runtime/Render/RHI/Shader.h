#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "VertexBuffer.h"
#include "Texture.h"

namespace Zero
{


	struct FPipelineStateDesc;
	enum class EShaderType
	{
		ST_VERTEX,
		ST_PIXEL,
		ST_COMPUTE
	};

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
		bool bUseAlphaBlending = false;
		FVertexBufferLayout VertexBufferLayout;
		int NumRenderTarget = 2;
		FFrameBufferTexturesFormats Formats = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::INT32, //   For Picking
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::DEPTH32F
		};

		std::string ShaderName;
		FShaderDefines ShaderDefines;
		bool bCreateVS = true;
		std::string VSEntryPoint = "VS";
		bool bCreatePS = true;
		std::string PSEntryPoint = "PS";
		bool bCreateCS = false;
		std::string CSEntryPoint = "CS";
	};
		
	class FShader
	{
	public:
		FShader(const std::string ShaderID, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		FShader(const FShaderDesc& Desc);
		virtual ~FShader() {}
		virtual void Use() = 0;
		virtual void CreateBinder() = 0;

		virtual Ref<IShaderBinder> GetBinder() { return m_ShaderBinder; }

	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;
	};
	
}