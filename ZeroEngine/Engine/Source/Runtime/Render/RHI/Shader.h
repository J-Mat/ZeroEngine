#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "VertexBuffer.h"

namespace Zero
{
	struct FShaderDesc
	{
		bool useAlphaBlending = false;
		FVertexBufferLayout inputLayout;
		int NumRenderTarget = 1;
	};


	struct FPipelineStateDesc;
	
	class IShader
	{
		IShader(std::string FileName, const FShaderDesc& ShaderDesc, const FShaderBinderDesc& BinderDesc);
		virtual ~IShader() {}
		virtual void Use() = 0;
		virtual void CreateBinder() = 0;
		virtual void SetVertexBufferLayout() = 0;
		virtual void UsePipelineState(const FPipelineStateDesc& Desc) = 0;

		Ref<ShaderBinder> GetBinder() { return m_ShaderBinder; }
		std::string ShaderID;

	protected:
		Ref<ShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;
	};
	
}