#include "PSORegister.h"
#include "Render/RHI/Shader.h"
#include "Render/RHI/ShaderBinder.h"
#include "Render/RenderConfig.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Platform/DX12/PSO/GenerateMipsPSO.h"

namespace Zero
{
	void FPSORegister::RegisterDefaultPSO()
	{
		{
			FShaderDesc ShaderDesc 
			{
				.ShaderName = PSO_FORWARDLIT,
			};

			FPSODescriptor ForwadLitDesc{
				.PSOType =	EPSOType::PT_Normal
			};

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
					EResourceFormat::B8G8R8A8_UNORM
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
					EResourceFormat::B8G8R8A8_UNORM
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
			FPSODescriptor ShadowDesc{
				.PSOType = EPSOType::PT_Depth
			};
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

	void FPSORegister::RegisterComputeShader()
	{
		{
			struct FComputeShaderDesc GenerteMipShader
			{
				.ShaderName = GENERATE_MIP_SHADER,
					.BlockSize_X = 8,
					.BlockSize_Y = 8,
					.BlockSize_Z = 1
			};
			FRenderer::GraphicFactroy->CreateComputeShader(GenerteMipShader);
		}
		{
			struct FComputeShaderDesc GenerteMipShader
			{
				.ShaderName = GENERATE_MIP_SHADER_TEST,
					.BlockSize_X = 8,
					.BlockSize_Y = 8,
					.BlockSize_Z = 1
			};
			FRenderer::GraphicFactroy->CreateComputeShader(GenerteMipShader);
		}
	}

	void FPSORegister::RegisterTestPSO()
	{
		FShaderDesc TestShaderDesc
		{
			.ShaderName = "Shader\\Test.hlsl",
			.NumRenderTarget = 1,
			.Formats = {
				EResourceFormat::B8G8R8A8_UNORM
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
