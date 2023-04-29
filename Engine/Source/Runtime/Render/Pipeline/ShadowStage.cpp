#include "ShadowStage.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RendererAPI.h"
#include "World/World.h"
#include "World/LightManager.h"
#include "Render/Moudule/PSOCache.h"
#include "Render/Moudule/Material.h"

namespace Zero
{
	void FShadowStage::RegisterDebugShadowMap()
	{
		FRenderTarget2DDesc Desc
		{
			.RenderTargetName = "ShadowMapDebug",
			.Width = 2048,
			.Height = 2048,
			.ColorFormat = {
				EResourceFormat::B8G8R8A8_UNORM
			},
			.bNeedDepth = false
		};
		m_ShadowMapDebugRenderTarget = FGraphic::GetDevice()->CreateRenderTarget2D(Desc);
		TLibrary<FRenderTarget2D>::Push(RENDER_STAGE_SHADOWMAP_DEBUG, m_ShadowMapDebugRenderTarget);

		Ref<FRenderItemPool> RenderItemPool = UWorld::GetCurrentWorld()->GetDIYRenderItemPool();
			
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::Get().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);

		m_ShadowMapDebugItem = RenderItemPool->Request();
		m_ShadowMapDebugItem->m_Mesh = FGraphic::GetDevice()->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
		m_ShadowMapDebugItem->m_SubMesh = *m_ShadowMapDebugItem->m_Mesh->begin();
		m_ShadowMapDebugItem->m_Material = CreateRef<FMaterial>(false);
		m_ShadowMapDebugItem->m_PsoID = EPsoID::ShadowDebug;
		m_ShadowMapDebugItem->m_Material->SetShader(m_ShadowMapDebugItem->GetPsoObj()->GetPSODescriptor().Shader);
	}

	void FShadowStage::RegisterShadowMap()
	{
		m_ShadowMapRenderTargets.resize(FLightManager::Get().GetMaxDirectLightsNum());
		for (uint32_t Index = 0; Index < FLightManager::Get().GetMaxDirectLightsNum(); ++Index)
		{
			FRenderTarget2DDesc Desc
			{
				.RenderTargetName = "ShadowMap",
				.Width = 2048,
				.Height = 2048,
				.ColorFormat = {
				},
				.bNeedDepth = true,
				.DepthFormat = EResourceFormat::D24_UNORM_S8_UINT 
			};
			m_ShadowMapRenderTargets[Index] = FGraphic::GetDevice()->CreateRenderTarget2D(Desc);
			std::string ShadowMapName = std::format("{0}_{1}", RENDER_STAGE_SHADOWMAP, Index);
			TLibrary<FRenderTarget2D>::Push(ShadowMapName, m_ShadowMapRenderTargets[Index]);
		}
	}

	void FShadowStage::RenderShadowMapDebug()
	{
		m_ShadowMapDebugRenderTarget->Bind(CommandListHandle);

		m_ShadowMapDebugItem->PreRender(CommandListHandle);
		if (FLightManager::Get().GetDirectLights().size() > 0)
		{
			m_ShadowMapDebugItem->m_Material->SetTexture2D("_gShadowMaps", m_ShadowMapRenderTargets[0]->GetDepthTexture());
		}
		m_ShadowMapDebugItem->Render(CommandListHandle);
		m_ShadowMapDebugRenderTarget->UnBind(CommandListHandle);
	}

	void FShadowStage::RenderShadowMap()
	{
		const auto& DirectLights = FLightManager::Get().GetDirectLights();
		for (int32_t Index = 0; Index < DirectLights.size(); ++Index)
		{
			UDirectLightActor* DirectLight = DirectLights[Index];
			m_ShadowMapRenderTargets[Index]->Bind(CommandListHandle);
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(ERenderLayer::Shadow);
			UDirectLightComponnet*  DirectLightComponent = DirectLight->GetComponent<UDirectLightComponnet>();
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->PreRender(CommandListHandle);
				RenderItem->Render(CommandListHandle);
			}
			m_ShadowMapRenderTargets[Index]->UnBind(CommandListHandle);
		}
	}

	void FShadowStage::OnAttach()
	{
		RegisterDebugShadowMap();
		RegisterShadowMap();
	}

	void FShadowStage::OnDetach()
	{
	}

	void FShadowStage::OnDraw()
	{
		RenderShadowMap();
		//RenderShadowMapDebug();
	}
}
