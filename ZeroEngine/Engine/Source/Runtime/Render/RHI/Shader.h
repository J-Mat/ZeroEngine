#pragma once
#include "Core.h"
#include "ShaderData.h"
#include "ShaderBinder.h"
#include "VertexBuffer.h"

namespace Zero
{

	// An enum for root signature parameters.
	// I'm not using scoped enums to avoid the explicit cast that would be required
	// to use these as root indices in the root signature.
	enum ERootParameters
	{
		PerObjCB,         // ConstantBuffer<Mat> MatCB : register(b0);
		CameraCB,         // ConstantBuffer<Mat> MatCB : register(b0);
		MaterialCB,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
		FrameConstantCB,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
		PointLights,        // StructuredBuffer<PointLight> PointLights : register( t0 );
		SpotLights,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
		Texture2Ds,         // Texture2D DiffuseTexture : register( t2 );
		CubeTextures,
		NumRootParameters
	};

	struct FShaderDesc
	{
		bool bUseAlphaBlending = false;
		FVertexBufferLayout VertexBufferLayout;
		int NumRenderTarget = 1;
	};


	struct FPipelineStateDesc;
	
	class IShader
	{
	public:
		IShader(const FShaderBinderDesc& BinderDesc, const FShaderDesc& Desc);
		virtual ~IShader() {}
		virtual void Use() = 0;
		virtual void CreateBinder() = 0;

		virtual Ref<IShaderBinder> GetBinder() { return m_ShaderBinder; }
		std::string ShaderID;

	protected:
		Ref<IShaderBinder> m_ShaderBinder;
		FShaderDesc m_ShaderDesc;
		FShaderBinderDesc m_ShaderBinderDesc;
	};
	
}