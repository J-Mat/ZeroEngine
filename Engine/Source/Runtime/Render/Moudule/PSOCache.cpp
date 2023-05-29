#include "PSOCache.h"
#include "Render/RHI/Shader/Shader.h"
#include "Render/RHI/Shader/ShaderBinder.h"
#include "Render/RenderConfig.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
#include "Render/RHI/ComputePipelineStateObject.h"
#include "Platform/DX12/PSO/GenerateMipsPSO.h"
#include "Render/RHI/GraphicDevice.h"
#include "Render/Moudule/ShaderCache.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"

namespace Zero
{
	FPSOCache::FPSOCache()
	{
		m_GraphicPsoCache.resize(EGraphicPsoID::PSOCount);
		m_ComputePsoCache.resize(EComputePsoID::PSOCount);
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
		m_GraphicPsoCache[EPsoID::GlobalPso] = FGraphic::GetDevice()->CreateGraphicPSO(ErrorDesc);
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
			FGraphicPSODescriptor DirectLightDesc;
			DirectLightDesc.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc);
		 	m_GraphicPsoCache[EGraphicPsoID::DirectLight] = FGraphic::GetDevice()->CreateGraphicPSO(DirectLightDesc);
		}

		{
			FShaderDesc ShaderDesc 
			{
				.FileName = "Shader\\PointLight.hlsl",
				.ShaderID = EShaderID::PointLight
			};
			FGraphicPSODescriptor PointLightDesc;
			PointLightDesc.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc);
		 	m_GraphicPsoCache[EGraphicPsoID::PointLight] = FGraphic::GetDevice()->CreateGraphicPSO(PointLightDesc);
		}
	}

	void FPSOCache::RegisterSkyboxPSO()
	{
		FShaderDesc ShaderDesc 
		{
			.FileName = "Shader\\Skybox.hlsl",
			.ShaderID = EShaderID::SkyBox
		};
		FGraphicPSODescriptor SkyboxPSODesc
		{
			.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc),
			.DepthFunc = EComparisonFunc::LESS_EQUAL,
			.CullMode = ECullMode::CULL_MODE_FRONT
		};
		 m_GraphicPsoCache[EGraphicPsoID::Skybox] = FGraphic::GetDevice()->CreateGraphicPSO(SkyboxPSODesc);
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

			FGraphicPSODescriptor ForwadLitDesc{
				.PSOType =	EPSOType::PT_Normal
			};
			ForwadLitDesc.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc);
		 	m_GraphicPsoCache[EGraphicPsoID::ForwadLit] = FGraphic::GetDevice()->CreateGraphicPSO(ForwadLitDesc);
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
			FGraphicPSODescriptor IrradianceMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateGraphicShader(IBLIrradianceShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_GraphicPsoCache[EGraphicPsoID::IBLIrradiance] = FGraphic::GetDevice()->CreateGraphicPSO(IrradianceMapPSODesc);
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
			FGraphicPSODescriptor PrefilterMapPSODesc
			{
				.Shader = FShaderCache::Get().CreateGraphicShader(PrefilterMapShaderDesc),
				.bDepthEnable = false,
				.CullMode = ECullMode::CULL_MODE_FRONT,
			};
			m_GraphicPsoCache[EGraphicPsoID::IBLPrefilter] = FGraphic::GetDevice()->CreateGraphicPSO(PrefilterMapPSODesc);
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
			FGraphicPSODescriptor ShadowDesc{
				.PSOType = EPSOType::PT_Depth,
				.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc),
			};
			m_GraphicPsoCache[EGraphicPsoID::DirectLightShadowMap] = FGraphic::GetDevice()->CreateGraphicPSO(ShadowDesc);
		}


		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\ShadowDebug.hlsl",
				.ShaderID = EShaderID::ShadowDebug,
			};
			FGraphicPSODescriptor ShadowDesc
			{
				.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc),
				.bDepthEnable = false,
			};
			m_GraphicPsoCache[EGraphicPsoID::ShadowDebug] = FGraphic::GetDevice()->CreateGraphicPSO(ShadowDesc);
		}

		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\PointLightShadowMap.hlsl",
				.ShaderID = EShaderID::PointLightShadowMap,
			};
			FGraphicPSODescriptor ShadowDesc{
				.PSOType = EPSOType::PT_Depth,
				.Shader = FShaderCache::Get().CreateGraphicShader(ShaderDesc),
			};
			m_GraphicPsoCache[EGraphicPsoID::PointLightShadowMap] = FGraphic::GetDevice()->CreateGraphicPSO(ShadowDesc);
		}

		/*
		{
			FShaderDesc ShaderDesc
			{
				.FileName = "Shader\\Shadow\\PointLightShadowMap_test.hlsl",
				.ShaderID = EShaderID::PointLightShadowMap,
				.NumRenderTarget = 1,
				.Formats = {
					EResourceFormat::R8G8B8A8_UNORM
				},
			};
			FPSODescriptor ShadowDesc{
				.Shader = FShaderCache::Get().CreateShader(ShaderDesc),
			};
			m_GraphicPsoCache[EPsoID::PointLightShadowMap] = FGraphic::GetDevice()->CreateGraphicPSO(ShadowDesc);
		}
		*/
	}

	void FPSOCache::RegisterComputeShader()
	{
		{
			struct FShaderDesc PrefixSumTexShader
			{
				.FileName = "Shader\\Compute\\PrefixSumTex_CS.hlsl",
				.BlockSize_X = 1024,
				.BlockSize_Y = 1,
				.BlockSize_Z = 1
			};
			Ref<FShader> Shader = FGraphic::GetDevice()->CreateComputeShader(PrefixSumTexShader);
			FComputePSODescriptor PrefixSumPso = 
			{
				.Shader = Shader,
				.BlockSize_X = 1024,
				.BlockSize_Y = 1,
				.BlockSize_Z = 1
			};
			m_ComputePsoCache[EComputePsoID::PrefixSumTex] = FGraphic::GetDevice()->CreateComputePSO(PrefixSumPso);
		}

		{
			struct FShaderDesc GenerteMipShader
			{
				.FileName = GENERATE_MIP_SHADER,
				.BlockSize_X = 8,
				.BlockSize_Y = 8,
				.BlockSize_Z = 1
			};
			Ref<FShader> Shader = FGraphic::GetDevice()->CreateComputeShader(GenerteMipShader);
			TLibrary<FShader>::Push(GENERATE_MIP_SHADER_TEST, Shader);
		}

		{
			struct FShaderDesc GenerteMipShader
			{
				.FileName = GENERATE_MIP_SHADER_TEST,
				.BlockSize_X = 8,
				.BlockSize_Y = 8,
				.BlockSize_Z = 1
			};
			Ref<FShader> Shader = FGraphic::GetDevice()->CreateComputeShader(GenerteMipShader);
			TLibrary<FShader>::Push(GENERATE_MIP_SHADER_TEST, Shader);
		}
	}


	void FPSOCache::OnReCreateGraphicPSO(Ref<FShader> Shader)
	{
		for (uint32_t i = 0; i < m_GraphicPsoCache.size();++i)
		{
			if (!m_GraphicPsoCache[i])
			{
				continue;
			}
			auto& PsoDesc = m_GraphicPsoCache[i]->GetPSODescriptor();
			auto OldShader = PsoDesc.Shader;
			if (OldShader->GetDesc().ShaderID == Shader->GetDesc().ShaderID)
			{
				auto NewPsoDesc = PsoDesc;
				NewPsoDesc.Shader = Shader;
				m_GraphicPsoCache[i] = FGraphic::GetDevice()->CreateGraphicPSO(NewPsoDesc);
				m_PsoRecreateEvent.Broadcast(i);
			}
		}
	}

	Ref<FComputePipelineStateObject> FPSOCache::FetchComputePso(uint32_t PsoID)
	{
		return m_ComputePsoCache[PsoID];
	}

	Ref<FGraphicPipelineStateObject> Zero::FPSOCache::FetchGraphicPso(uint32_t PsoID)
	{
		return m_GraphicPsoCache[PsoID];
	}

}
