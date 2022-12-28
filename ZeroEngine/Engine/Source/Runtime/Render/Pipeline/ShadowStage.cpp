#include "ShadowStage.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RendererAPI.h"
#include "Render/Moudule/Material.h"
#include "World/World.h"
#include "World/LightManager.h"

namespace Zero
{
	void FShadowStage::OnAttach()
	{
		m_ShadowMapRenderTargets.resize(FLightManager::GetInstance().GetMaxDirectLightsNum());
		for (int32_t Index = 0; Index < FLightManager::GetInstance().GetMaxDirectLightsNum(); ++Index)
		{
			FRenderTarget2DDesc Desc
			{
				.Width = 2048,
				.Height = 2048,
				.Format = {
					ETextureFormat::DEPTH32F
				},
			};
			m_ShadowMapRenderTargets[Index] = FRenderer::GraphicFactroy->CreateRenderTarget2D(Desc);
			std::string ShadowMapName = std::format("{0}_{1}", RENDER_STAGE_SHADOWMAP, Index);
			TLibrary<FRenderTarget2D>::Push(ShadowMapName, m_ShadowMapRenderTargets[Index]);
		}
	}

	void FShadowStage::OnDetach()
	{
	}

	void FShadowStage::OnDraw()
	{
		const auto& DirectLights = FLightManager::GetInstance().GetDirectLights();
		for (int32_t Index = 0; Index < DirectLights.size(); ++Index)
		{
			UDirectLightActor* DirectLight = DirectLights[Index];
			m_ShadowMapRenderTargets[Index]->Bind();
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_SHAWODOW);
			UDirectLightComponnet*  DirectLightComponent = DirectLight->GetComponent<UDirectLightComponnet>();
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_PipelineStateObject->Bind();
				const auto& PSODesc = RenderItem->m_PipelineStateObject->GetPSODescriptor();
				RenderItem->m_Material->Tick();
				RenderItem->m_Material->SetPass();
				RenderItem->m_Material->SetCameraProjectViewMat("ProjectionView", DirectLightComponent->GetViewProject());
				RenderItem->m_Material->OnDrawCall();
				RenderItem->OnDrawCall();
			}
			m_ShadowMapRenderTargets[Index]->UnBind();
		}
	}
}
