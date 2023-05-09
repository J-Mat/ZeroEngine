#include "PSOCache.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RenderConfig.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Platform/DX12/PSO/GenerateMipsPSO.h"
#include "Render/RHI/GraphicDevice.h"
#include "Render/Moudule/ShaderCache.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"

namespace Zero
{
	FPSOCache::FPSOCache()
	{
		m_PsoCache.resize(EPsoID::PSOCount);
	}

	void FPSOCache::RegisterErrorPSO()
	{
		/*
		FShaderDesc ShaderDesc 
		{
			.FileName = "Shader\\Error.hlsl",
			.ShaderID = EShaderID::Error,
		};

		FPSODescriptor ErrorDesc{
			.PSOType =	EPSOType::PT_Normal
		};

		ErrorDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		m_PsoCache[EPsoID::GlobalPso] = FGraphic::GetDevice()->CreatePSO(ErrorDesc);
	*/
	}

	void FPSOCache::RegisterDefaultPSO()
	{

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\DirectLight.hlsl",
				.ShaderID = EShaderID::DirectLight
			};
			FPSODescriptor DirectLightDesc;
			DirectLightDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		 	m_PsoCache[EPsoID::DirectLight] = FGraphic::GetDevice()->CreatePSO(DirectLightDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\PointLight.hlsl",
				.ShaderID = EShaderID::PointLight
			};
			FPSODescriptor PointLightDesc;
			PointLightDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		 	m_PsoCache[EPsoID::PointLight] = FGraphic::GetDevice()->CreatePSO(PointLightDesc);
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
		 m_PsoCache[EPsoID::Skybox] = FGraphic::GetDevice()->CreatePSO(SkyboxPSODesc);
	}

	void FPSOCache::RegsiterForwardLitPSO()
	{
		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\ForwardLit.hlsl",
				.ShaderID = EShaderID::ForwardLit,
				.DefinesMap = {}
			};
			
			auto* SceneSettings = FSettingManager::Get().FecthSettings<USceneSettings>(USceneSettings::StaticGetObjectName());
			switch (SceneSettings->m_ShadowType)
			{
			case EShadowType::PCF:
				break;
			case EShadowType::PCSS:
				ShaderDesc.SetDefine("USE_PCSS", "1");
				break;
			default:
				break;
			}

			FPSODescriptor ForwadLitDesc{
				.PSOType =	EPSOType::PT_Normal
			};
			ForwadLitDesc.Shader = FShaderCache::Get().CreateShader(ShaderDesc);
		 	m_PsoCache[EPsoID::ForwadLit] = FGraphic::GetDevice()->CreatePSO(ForwadLitDesc);
		}
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
					EResourceFormat::R8G8B8A8_UNORM
				},
			};
			FPSODescriptor IrradianceMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateShader(IBLIrradianceShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_PsoCache[EPsoID::IBLIrradiance] = FGraphic::GetDevice()->CreatePSO(IrradianceMapPSODesc);
		}

		{
			FShaderDesc PrefilterMapShaderDesc
			{
				.FileName = "Shader\\IBL\\IBLPrefilter.hlsl",
				.ShaderID = EShaderID::IBLPrefilter,
				.NumRenderTarget = 1,
				.Formats = {
					EResourceFormat::R8G8B8A8_UNORM
				},
			};
			FPSODescriptor PrefilterMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateShader(PrefilterMapShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_PsoCache[EPsoID::IBLPrefilter] = FGraphic::GetDevice()->CreatePSO(PrefilterMapPSODesc);
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
			m_PsoCache[EPsoID::ShadowMap] = FGraphic::GetDevice()->CreatePSO(ShadowDesc);
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
			m_PsoCache[EPsoID::ShadowDebug] = FGraphic::GetDevice()->CreatePSO(ShadowDesc);
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
			FGraphic::GraphicFactroy->CreateComputeShader(GenerteMipShader);
		}

		{
			struct FComputeShaderDesc GenerteMipShader
			{
				.ShaderName = GENERATE_MIP_SHADER_TEST,
					.BlockSize_X = 8,
					.BlockSize_Y = 8,
					.BlockSize_Z = 1
			};
			FGraphic::GraphicFactroy->CreateComputeShader(GenerteMipShader);
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
				m_PsoCache[i] = FGraphic::GetDevice()->CreatePSO(NewPsoDesc);
				m_PsoRecreateEvent.Broadcast(i);
			}
		}
	}

	Ref<FPipelineStateObject> Zero::FPSOCache::Fetch(uint32_t PsoID)
	{
		return m_PsoCache[PsoID];
	}

}