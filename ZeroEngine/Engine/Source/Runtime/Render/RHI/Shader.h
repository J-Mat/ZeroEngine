#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "VertexBuffer.h"
#include "Texture.h"

namespace Zero
{
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
	};


	struct FPipelineStateDesc;
	
	class IShader
	{
	public:
		IShader(const std::string ShaderID, const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		virtual ~IShader() {}
		virtual void Use() = 0;
		virtual void CreateBinder() = 0;

		virtual Ref<IShaderBinder> GetBinder() { return m_ShaderBinder; }
		std::string m_ShaderID;

	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;
	};
	
}