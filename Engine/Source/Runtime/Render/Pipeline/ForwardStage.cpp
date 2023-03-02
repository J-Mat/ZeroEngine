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
			.ColorFormat = {
				EResourceFormat::R8G8B8A8_UNORM,
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
		//m_IBLMoudule = CreateRef<FImageBasedLighting>(RenderItemPool->Request(), Items);
	}

	void FForwardStage::OnDetach()
	{
	}

	void FForwardStage::RawRenderLayer(uint32_t RenderLayerType)
	{
		auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RenderLayerType);
		for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
		{
			RenderItem->PreRender(m_CommandListHandle);
			RenderItem->Render(m_CommandListHandle);
		}
	}

	void FForwardStage::ForwarLitRender()
	{
		static auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_OPAQUE);
		static auto LUTTexture2D = TLibrary<FTexture2D>::Fetch(IBL_BRDF_LUT);
		static std::string ShadowMapID = std::format("{0}_{1}",  RENDER_STAGE_SHADOWMAP,  0);
		static Ref<FRenderTarget2D> ShadowMapRenderTarget = TLibrary<FRenderTarget2D>::Fetch(ShadowMapID);
		if (ShadowMapRenderTarget == nullptr)
		{
			ShadowMapRenderTarget = TLibrary<FRenderTarget2D>::Fetch(ShadowMapID);
		}
		for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
		{
			RenderItem->PreRender(m_CommandListHandle);
			const auto& PSODesc = RenderItem->m_PipelineStateObject->GetPSODescriptor();
			const std::string& ShaderName = PSODesc.Shader->GetDesc().FileName;
			if (m_bGenerateIrradianceMap && !RenderItem->m_Material->IsSetIBL())
			{
				RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", m_IBLMoudule->GetIrradianceRTCube()->GetColorCubemap());
				RenderItem->m_Material->SetTextureCubemapArray("IBLPrefilterMaps", m_IBLMoudule->GetPrefilterEnvMapTextureCubes());
				RenderItem->m_Material->SetTexture2D("_BrdfLUT", LUTTexture2D);
				RenderItem->m_Material->SetIBL(true);
			}

			if (ShadowMapRenderTarget != nullptr)
			{
				//RenderItem->m_Material->SetTexture2D("_gShadowMap", ShadowMapRenderTarget->GetDepthTexture());
			}

			RenderItem->Render(m_CommandListHandle);
		}
	}

	void FForwardStage::OnDraw()
	{
		m_CommandListHandle = FRenderer::GetDevice()->GetSingleThreadCommadList();
		static auto* Settings = FSettingManager::Get().FecthSettings<USceneSettings>(USceneSettings::StaticGetObjectName());
		
		if (Settings->m_bUseSkyBox)
		{ 
			if (!m_bGenerateIrradianceMap)
			{
				m_IBLMoudule->PreCaculate();
				m_bGenerateIrradianceMap = true;
			}
		}


		m_RenderTarget->Bind(m_CommandListHandle);
		if (Settings->m_bUseSkyBox)
		{
			RawRenderLayer(RENDERLAYER_SKYBOX);
		}
		RawRenderLayer(RENDERLAYER_LIGHT);
		ForwarLitRender();
		m_RenderTarget->UnBind(m_CommandListHandle);
	}
}
