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
		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = PSO_FORWARDLIT,
			};

			FPSODescriptor ForwadLitDesc;
			ForwadLitDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO(PSO_FORWARDLIT, ForwadLitDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = "Shader\\DirectLight.hlsl",
			};
			FPSODescriptor DirectLightDesc;
			DirectLightDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO(PSO_DIRECT_LIGHT, DirectLightDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = "Shader\\PointLight.hlsl",
			};
			FPSODescriptor PointLightDesc;
			PointLightDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO(PSO_POINT_LIGHT, PointLightDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = "Shader\\GGX.hlsl",
			};
			FPSODescriptor GGXDesc;
			GGXDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO("Shader\\GGX.hlsl", GGXDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = "Shader\\NDF.hlsl",
			};
			FPSODescriptor NDFDesc;
			NDFDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO("Shader\\NDF.hlsl", NDFDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = "Shader\\Lighting.hlsl",
			};
			FPSODescriptor LightingDesc;
			LightingDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO("Shader\\Lighting.hlsl", LightingDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = "Shader\\PBR_Texture.hlsl",
			};
			FPSODescriptor PBRTextureDesc;
			PBRTextureDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO("Shader\\PBR_Texture.hlsl", PBRTextureDesc);
		}
	}

	void FPSORegister::RegisterSkyboxPSO()
	{
		FShaderDesc ShaderDesc 
		{
			.ShaderName = "Shader\\Skybox.hlsl",
		};
		FPSODescriptor SkyboxPSODesc
		{
			.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc),
			.DepthFunc = EComparisonFunc::LESS_EQUAL,
			.CullMode = ECullMode::CULL_MODE_FRONT
		};
		FRenderer::GraphicFactroy->CreatePSO(PSO_SKYBOX, SkyboxPSODesc);

	}

	void FPSORegister::RegisterIBLPSO()
	{
		{
			FShaderDesc IBLIrradianceShaderDesc
			{
				.ShaderName = "Shader\\IBL\\IBLIrradiance.hlsl",
				.NumRenderTarget = 1,
				.Formats = {
					ETextureFormat::R8G8B8A8,
				},
			};
			FPSODescriptor IrradianceMapPSODesc
			{
				.Shader = FRenderer::GraphicFactroy->CreateShader(IBLIrradianceShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			FRenderer::GraphicFactroy->CreatePSO("Shader\\IBL\\IBLIrradiance.hlsl", IrradianceMapPSODesc);
		}

		{
			FShaderDesc PrefilterMapShaderDesc
			{
				.ShaderName = "Shader\\IBL\\IBLPrefilter.hlsl",
				.NumRenderTarget = 1,
				.Formats = {
					ETextureFormat::R8G8B8A8,
				},
			};
			FPSODescriptor PrefilterMapPSODesc
			{
				.Shader = FRenderer::GraphicFactroy->CreateShader(PrefilterMapShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			FRenderer::GraphicFactroy->CreatePSO(PrefilterMapShaderDesc.ShaderName, PrefilterMapPSODesc);
		}
	}

	void FPSORegister::RegisterShadowPSO()
	{
		{
			FShaderDesc ShaderDesc
			{
				.ShaderName = "Shader\\Shadow\\DirectLightShadowMap.hlsl",
			};
			FPSODescriptor ShadowDesc;
			ShadowDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO(PSO_SHADOWMAP, ShadowDesc);
		}

		{
			FShaderDesc ShaderDesc
			{
				.ShaderName = "Shader\\Shadow\\ShadowDebug.hlsl",
			};
			FPSODescriptor ShadowDesc
			{
				.bDepthEnable = false,
			};
			ShadowDesc.Shader = FRenderer::GraphicFactroy->CreateShader(ShaderDesc);
			FRenderer::GraphicFactroy->CreatePSO(ShaderDesc.ShaderName, ShadowDesc);
		}
	}

	void FPSORegister::RegisterTestPSO()
	{
		FShaderDesc TestShaderDesc
		{
			.ShaderName = "Shader\\Test.hlsl",
			.NumRenderTarget = 1,
			.Formats = {
				ETextureFormat::R8G8B8A8,
			},
		};
		FPSODescriptor TestPSODesc
		{
			.Shader = FRenderer::GraphicFactroy->CreateShader(TestShaderDesc),
			.bDepthEnable = false,
		};
		FRenderer::GraphicFactroy->CreatePSO("Shader\\Test.hlsl", TestPSODesc);
	}
}
