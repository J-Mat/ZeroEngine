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

		FPSODescriptor LightingDesc;
		LightingDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\Lighting.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO("Shader\\Lighting.hlsl", LightingDesc);

		FPSODescriptor PBRTextureDesc;
		PBRTextureDesc.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\PBR_Texture.hlsl", ShaderDesc);
		FRenderer::GraphicFactroy->CreatePSO("Shader\\PBR_Texture.hlsl", PBRTextureDesc);
	}

	void FPSORegister::RegisterSkyboxPSO()
	{
		FShaderDesc ShaderDesc;
		FPSODescriptor SkyboxPSODesc
		{
			.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\Skybox.hlsl", ShaderDesc),
			.DepthFunc = EComparisonFunc::LESS_EQUAL,
			.CullMode = ECullMode::CULL_MODE_FRONT,
		};
		FRenderer::GraphicFactroy->CreatePSO(PSO_SKYBOX, SkyboxPSODesc);


		FShaderDesc IBLIrradianceShaderDesc
		{
			.NumRenderTarget = 1,
			.Formats = {
				ETextureFormat::R8G8B8A8,
			},
		};
		FPSODescriptor IrradiancePSODesc
		{
			.Shader = FRenderer::GraphicFactroy->CreateShader("Shader\\IBLIrradiance.hlsl", IBLIrradianceShaderDesc),
			.bDepthEnable = false,
			.CullMode = ECullMode::CULL_MODE_NONE,
		};
		FRenderer::GraphicFactroy->CreatePSO("Shader\\IBLIrradiance.hlsl", IrradiancePSODesc);
	}
}
