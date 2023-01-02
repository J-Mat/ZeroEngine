#include "ForwardStage.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/RHI/SwapChain.h"
#include "Render/RHI/RenderTarget.h"
#include "World/World.h"
#include "World/LightManager.h"
#include "Render/RenderConfig.h"
#include "Data/Settings/SettingsManager.h"
#include "Data/Settings/SceneSettings.h"
#include "Render/Moudule/ImageBasedLighting.h"

namespace Zero
{
	void FForwardStage::OnAttach()
	{
		FRenderTarget2DDesc Desc
		{
			.RenderTargetName = "ForwardLit",
			.Format = {
				ETextureFormat::R8G8B8A8,
				ETextureFormat::DEPTH32F
			}
		};
		m_RenderTarget = FRenderer::GraphicFactroy->CreateRenderTarget2D(Desc);
		TLibrary<FRenderTarget2D>::Push(RENDER_STAGE_FORWARD, m_RenderTarget);

		Ref<FRenderItemPool> RenderItemPool = UWorld::GetCurrentWorld()->GetDIYRenderItemPool();
		Ref<FRenderItem> Items[5];
		for (int i = 0; i < 5; ++i)
		{
			Items[i] = RenderItemPool->Request();
		}
		m_IBLMoudule = CreateRef<FImageBasedLighting>(RenderItemPool->Request(), Items);
	}

	void FForwardStage::OnDetach()
	{
	}

	void FForwardStage::OnDraw()
	{
		static auto* Settings = FSettingManager::GetInstance().FecthSettings<USceneSettings>(USceneSettings::StaticGetObjectName());
		
		if (Settings->m_bUseSkyBox)
		{ 
			if (!m_bGenerateIrradianceMap)
			{
				m_IBLMoudule->PreCaculate();
				m_bGenerateIrradianceMap = true;
			}
		}


		m_RenderTarget->Bind();
		if (Settings->m_bUseSkyBox)
		{
			static auto SkyboxPSO = TLibrary<FPipelineStateObject>::Fetch(PSO_SKYBOX);
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_SKYBOX);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_PipelineStateObject = SkyboxPSO;
				RenderItem->PreRender();
				RenderItem->Render();
			}
		}

		{
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_OPAQUE);
			static auto LUTTexture2D = TLibrary<FTexture2D>::Fetch(IBL_BRDF_LUT);
			static std::string ShadowMapID = std::format("{0}_{1}",  RENDER_STAGE_SHADOWMAP,  "_0");
			static Ref<FRenderTarget2D> ShadowMapRenderTarget = TLibrary<FRenderTarget2D>::Fetch(ShadowMapID);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->PreRender();
				const auto& PSODesc = RenderItem->m_PipelineStateObject->GetPSODescriptor();
				const std::string& ShaderName = PSODesc.Shader->GetDesc().ShaderName;
				if (m_bGenerateIrradianceMap && !RenderItem->m_Material->IsSetIBL() && ShaderName == PSO_FORWARDLIT)
				{
					RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", m_IBLMoudule->GetIrradianceRTCube()->GetColorCubemap());
					RenderItem->m_Material->SetTextureCubemapArray("IBLPrefilterMaps", m_IBLMoudule->GetPrefilterEnvMapTextureCubes());
					RenderItem->m_Material->SetTexture2D("_BrdfLUT", LUTTexture2D);
					RenderItem->m_Material->SetIBL(true);
				}

				RenderItem->Render();
			}
		}
		m_RenderTarget->UnBind();
	}
}
