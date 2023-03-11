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
	FPSOCache::FPSOCache()
	{
		m_PsoCache.resize(EPsoID::PSOCount);
	}

	void FPSOCache::RegisterDefaultPSO()
	{
		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\ForwardLit.hlsl",
				.ShaderID = EShaderID::ForwardLit,
			};

			FPSODescriptor ForwadLitDesc{
				.PSOType =	EPSOType::PT_Normal
			};

			ForwadLitDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		 	m_PsoCache[EPsoID::ForwadLit] = FRenderer::GetDevice()->CreatePSO(ForwadLitDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\DirectLight.hlsl",
				.ShaderID = EShaderID::DirectLight
			};
			FPSODescriptor DirectLightDesc;
			DirectLightDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		 	m_PsoCache[EPsoID::DirectLight] = FRenderer::GetDevice()->CreatePSO(DirectLightDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\PointLight.hlsl",
				.ShaderID = EShaderID::PointLight
			};
			FPSODescriptor PointLightDesc;
			PointLightDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		 	m_PsoCache[EPsoID::PointLight] = FRenderer::GetDevice()->CreatePSO(PointLightDesc);
		}
	}

	void FPSOCache::RegisterSkyboxPSO()
	{
		FShaderDesc ShaderDesc 
		{
			.FileName = "Shader\\Skybox.hlsl",
			.ShaderID = EShaderID::SkyBox
		};
		FPSODescriptor SkyboxPSODesc
		{
			.Shader = FShaderCache::Get().CreateShader(ShaderDesc),
			.DepthFunc = EComparisonFunc::LESS_EQUAL,
			.CullMode = ECullMode::CULL_MODE_FRONT
		};
		 m_PsoCache[EPsoID::Skybox] = FRenderer::GetDevice()->CreatePSO(SkyboxPSODesc);
	}

	void FPSOCache::RegisterIBLPSO()
	{
		{
			FShaderDesc IBLIrradianceShaderDesc
			{
				.FileName = "Shader\\IBL\\IBLIrradiance.hlsl",
				.ShaderID = EShaderID::IBLIrradiance,
				.NumRenderTarget = 1,
				.Formats = {
					EResourceFormat::B8G8R8A8_UNORM
				},
			};
			FPSODescriptor IrradianceMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateShader(IBLIrradianceShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_PsoCache[EPsoID::IBLIrradiance] = FRenderer::GetDevice()->CreatePSO(IrradianceMapPSODesc);
		}

		{
			FShaderDesc PrefilterMapShaderDesc
			{
				.FileName = "Shader\\IBL\\IBLPrefilter.hlsl",
				.ShaderID = EShaderID::IBLPrefilter,
				.NumRenderTarget = 1,
				.Formats = {
					EResourceFormat::B8G8R8A8_UNORM
				},
			};
			FPSODescriptor PrefilterMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateShader(PrefilterMapShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_PsoCache[EPsoID::IBLPrefilter] = FRenderer::GetDevice()->CreatePSO(PrefilterMapPSODesc);
		}
	}

	void FPSOCache::RegisterShadowPSO()
	{
		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\DirectLightShadowMap.hlsl",
				.ShaderID = EShaderID::DirectLightShadowMap,
			};
			FPSODescriptor ShadowDesc{
				.PSOType = EPSOType::PT_Depth,
				.Shader = FShaderCache::Get().CreateShader(ShaderDesc),
			};
			m_PsoCache[EPsoID::ShadowMap] = FRenderer::GetDevice()->CreatePSO(ShadowDesc);
		}

		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\ShadowDebug.hlsl",
				.ShaderID = EShaderID::ShadowDebug,
			};
			FPSODescriptor ShadowDesc
			{
				.Shader = FShaderCache::Get().CreateShader(ShaderDesc),
				.bDepthEnable = false,
			};
			m_PsoCache[EPsoID::ShadowDebug] = FRenderer::GetDevice()->CreatePSO(ShadowDesc);
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


	void FPSOCache::OnReCreatePso(Ref<FShader> Shader)
	{
		for (uint32_t i = 0; i < m_PsoCache.size();++i)
		{
			if (!m_PsoCache[i])
			{
				continue;
			}
			auto& PsoDesc = m_PsoCache[i]->GetPSODescriptor();
			auto OldShader = PsoDesc.Shader;
			if (OldShader->GetDesc().ShaderID == Shader->GetDesc().ShaderID)
			{
				auto NewPsoDesc = PsoDesc;
				NewPsoDesc.Shader = Shader;
				m_PsoCache[i] = FRenderer::GetDevice()->CreatePSO(NewPsoDesc);
				m_PsoRecreateEvent.Broadcast(i);
			}
		}
	}

	FPipelineStateObject* Zero::FPSOCache::Fetch(uint32_t PsoID)
	{
		return m_PsoCache[PsoID].get();
	}

}
