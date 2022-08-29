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
		FVertexBufferLayout VertexBufferLayout;
		int NumRenderTarget = 1;
	};


	struct FPipelineStateDesc;
	
	class IShader
	{
	public:
		IShader() = default;
		virtual ~IShader() {}
		virtual void Use() = 0;
		virtual void CreateBinder() = 0;
		virtual void SetVertexBufferLayout() = 0;
		virtual void UsePipelineState(const FPipelineStateDesc& Desc) = 0;

		Ref<IShaderBinder> GetBinder() { return m_ShaderBinder; }
		std::string ShaderID;

	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;
	};
	
}