#include "PSOCache.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RenderConfig.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Platform/DX12/PSO/GenerateMipsPSO.h"
#include "Render/RHI/GraphicDevice.h"
#include "Render/Moudule/ShaderCache.h"

namespace Zero
{
	Ref<FPipelineStateObject> FPSOCache::Fetch(EPipelineState PsoState)
	{
		auto Iter = m_PsoCache.find(PsoState);
		CORE_ASSERT(Iter != m_PsoCache.end(), "Can not find PSO State");
		return Iter->second;
	}

	void FPSOCache::RegisterDefaultPSO()
	{
		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\ForwardLit.hlsl",
			};

			FPSODescriptor ForwadLitDesc{
				.PSOType =	EPSOType::PT_Normal
			};

			ForwadLitDesc.Shader = FShaderCache::Get().CreateShader(EShaderID::ForwardLit, ShaderDesc);
		 	m_PsoCache[EPipelineState::ForwadLit] = FRenderer::GetDevice()->CreatePSO(ForwadLitDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\DirectLight.hlsl",
			};
			FPSODescriptor DirectLightDesc;
			DirectLightDesc.Shader = FShaderCache::Get().CreateShader(EShaderID::DirectLight, ShaderDesc);
		 	m_PsoCache[EPipelineState::DirectLight] = FRenderer::GetDevice()->CreatePSO(DirectLightDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\PointLight.hlsl",
			};
			FPSODescriptor PointLightDesc;
			PointLightDesc.Shader = FShaderCache::Get().CreateShader(EShaderID::PointLight, ShaderDesc);
		 	m_PsoCache[EPipelineState::PointLight] = FRenderer::GetDevice()->CreatePSO(PointLightDesc);
		}
	}

	void FPSOCache::RegisterSkyboxPSO()
	{
		FShaderDesc ShaderDesc 
		{
			.FileName = "Shader\\Skybox.hlsl",
		};
		FPSODescriptor SkyboxPSODesc
		{
			.Shader = FShaderCache::Get().CreateShader(EShaderID::SkyBox, ShaderDesc),
			.DepthFunc = EComparisonFunc::LESS_EQUAL,
			.CullMode = ECullMode::CULL_MODE_FRONT
		};
		 m_PsoCache[EPipelineState::Skybox] = FRenderer::GetDevice()->CreatePSO(SkyboxPSODesc);
	}

	void FPSOCache::RegisterIBLPSO()
	{
		{
			FShaderDesc IBLIrradianceShaderDesc
			{
				.FileName = "Shader\\IBL\\IBLIrradiance.hlsl",
				.NumRenderTarget = 1,
				.Formats = {
					EResourceFormat::B8G8R8A8_UNORM
				},
			};
			FPSODescriptor IrradianceMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateShader(EShaderID::IBLIrradiance, IBLIrradianceShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_PsoCache[EPipelineState::IBLIrradiance] = FRenderer::GetDevice()->CreatePSO(IrradianceMapPSODesc);
		}

		{
			FShaderDesc PrefilterMapShaderDesc
			{
				.FileName = "Shader\\IBL\\IBLPrefilter.hlsl",
				.NumRenderTarget = 1,
				.Formats = {
					EResourceFormat::B8G8R8A8_UNORM
				},
			};
			FPSODescriptor PrefilterMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateShader(EShaderID::IBLPrefilter, PrefilterMapShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_PsoCache[EPipelineState::IBLPrefilter] = FRenderer::GetDevice()->CreatePSO(PrefilterMapPSODesc);
		}
	}

	void FPSOCache::RegisterShadowPSO()
	{
		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\DirectLightShadowMap.hlsl",
			};
			FPSODescriptor ShadowDesc{
				.PSOType = EPSOType::PT_Depth,
				.Shader = FShaderCache::Get().CreateShader(EShaderID::DirectLightShadowMap, ShaderDesc),
			};
			m_PsoCache[EPipelineState::ShadowMap] = FRenderer::GetDevice()->CreatePSO(ShadowDesc);
		}

		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\ShadowDebug.hlsl",
			};
			FPSODescriptor ShadowDesc
			{
				.Shader = FShaderCache::Get().CreateShader(EShaderID::ShadowDebug, ShaderDesc),
				.bDepthEnable = false,
			};
			m_PsoCache[EPipelineState::ShadowDebug] = FRenderer::GetDevice()->CreatePSO(ShadowDesc);
		}
	}

	void FPSOCache::RegisterComputeShader()
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


	void FPSOCache::OnCreatePso(Ref<FShader> Shader)
	{
		for (auto [_, Pso] : m_PsoCache)
		{
			if (Pso->GetPSODescriptor().Shader->GetDesc().FileName == Shader->GetDesc().FileName)
			{
			}
		}
	}

}
