#include "PSORegister.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RenderConfig.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/PipelineStateObject.h"

namespace Zero
{
	void FPSORegister::RegisterDefaultPSO()
	{
		FShaderDesc ShaderDesc;

		FPSODescriptor ForwadLitDesc;
		ForwadLitDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\ForwardLit.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_FORWARDLIT, ForwadLitDesc);

		FPSODescriptor DirectLightDesc;
		DirectLightDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\DirectLight.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_DIRECT_LIGHT, DirectLightDesc);

		FPSODescriptor PointLightDesc;
		PointLightDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\PointLight.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_POINT_LIGHT, PointLightDesc);

		FPSODescriptor GGXDesc;
		GGXDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\GGX.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO("Shader\\GGX.hlsl", GGXDesc);

		FPSODescriptor NDFDesc;
		NDFDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\NDF.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO("Shader\\NDF.hlsl", NDFDesc);
	}

	void FPSORegister::RegisterSkyboxPSO()
	{
		FShaderDesc ShaderDesc;
		FPSODescriptor SkyboxPSODesc;
		SkyboxPSODesc.CullMode = ECullMode::CULL_MODE_FRONT;
		SkyboxPSODesc.DepthFunc = EComparisonFunc::LESS_EQUAL;
		SkyboxPSODesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\Skybox.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO(PSO_SKYBOX, SkyboxPSODesc);
	}
}
