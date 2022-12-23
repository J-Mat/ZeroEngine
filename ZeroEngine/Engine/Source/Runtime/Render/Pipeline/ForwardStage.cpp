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
		Ref<IDevice> Device = UWorld::GetCurrentWorld()->GetDevice();
		FRenderTarget2DDesc Desc;
		Desc.Format = {
			ETextureFormat::R8G8B8A8,
			ETextureFormat::None, //   For Picking
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::None,
			ETextureFormat::DEPTH32F
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
		m_RenderTarget->ClearBuffer();

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
			SkyboxPSO->Bind();
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_SKYBOX);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_Material->Tick();
				RenderItem->m_Material->SetPass();
				RenderItem->m_Material->OnDrawCall();
				RenderItem->OnDrawCall();
			}
		}

		{
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_OPAQUE);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_PipelineStateObject->Bind();
				if (m_bGenerateIrradianceMap)
				{
					//RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", m_IBLMoudule->GetIrradianceRTCube()->GetColorCubemap());
					RenderItem->m_Material->SetTextureCubemapArray("IBLPrefilterMaps", m_IBLMoudule->GetPrefilterEnvMapTextureCubes());
					//RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", m_IBLMoudule->GetPrefilterEnvMapTextureCubeIndex(1));
					//RenderItem->m_Material->SetTextureCubemap("IBLPrefilterMaps", m_IBLMoudule->GetPrefilterEnvMapTextureCube(0));
				}
				RenderItem->m_Material->Tick();
				RenderItem->m_Material->SetPass();
				RenderItem->m_Material->OnDrawCall();
				RenderItem->OnDrawCall();
			}
		}
		m_RenderTarget->UnBind();
	}
}
