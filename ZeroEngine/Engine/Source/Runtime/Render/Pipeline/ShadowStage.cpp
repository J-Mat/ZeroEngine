#include "ShadowStage.h"
#include "Render/RHI/RenderTarget.h"
#include "Render/RendererAPI.h"
#include "Render/Moudule/Material.h"
#include "World/World.h"
#include "World/LightManager.h"

namespace Zero
{
	void FShadowStage::RegisterDebugShadowMap()
	{
		FRenderTarget2DDesc Desc
		{
			.RenderTargetName = "ShadowMapDebug",
			.Width = 512,
			.Height = 512,
			.Format = {
				ETextureFormat::R8G8B8A8
			}
		};
		m_ShadowMapDebugRenderTarget = FRenderer::GraphicFactroy->CreateRenderTarget2D(Desc);
		TLibrary<FRenderTarget2D>::Push(RENDER_STAGE_SHADOWMAP_DEBUG, m_ShadowMapDebugRenderTarget);
		Ref<FRenderItemPool> RenderItemPool = UWorld::GetCurrentWorld()->GetDIYRenderItemPool();
			
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);

		m_ShadowMapDebugItem = RenderItemPool->Request();
		m_ShadowMapDebugItem->m_Mesh = FRenderer::GraphicFactroy->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
		m_ShadowMapDebugItem->m_SubMesh = *m_ShadowMapDebugItem->m_Mesh->begin();
		m_ShadowMapDebugItem->m_Material = CreateRef<FMaterial>(false);
		m_ShadowMapDebugItem->m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch("Shader\\Shadow\\ShadowDebug.hlsl");
		m_ShadowMapDebugItem->m_Material->SetShader(m_ShadowMapDebugItem->m_PipelineStateObject->GetPSODescriptor().Shader);
	}

	void FShadowStage::RegisterShadowMap()
	{
		m_ShadowMapRenderTargets.resize(FLightManager::GetInstance().GetMaxDirectLightsNum());
		for (uint32_t Index = 0; Index < FLightManager::GetInstance().GetMaxDirectLightsNum(); ++Index)
		{
			FRenderTarget2DDesc Desc
			{
				.RenderTargetName = "ShadowMap",
				.Width = 1024,
				.Height = 1024,
				.Format = {
					ETextureFormat::DEPTH32F
				},
			};
			m_ShadowMapRenderTargets[Index] = FRenderer::GraphicFactroy->CreateRenderTarget2D(Desc);
			std::string ShadowMapName = std::format("{0}_{1}", RENDER_STAGE_SHADOWMAP, Index);
			TLibrary<FRenderTarget2D>::Push(ShadowMapName, m_ShadowMapRenderTargets[Index]);
		}
	}

	void FShadowStage::RenderShadowMapDebug()
	{
		//static auto DefaultTexture = FRenderer::GraphicFactroy->GetOrCreateTexture2D("Texture\\yayi.png");
		m_ShadowMapDebugRenderTarget->Bind();

		m_ShadowMapDebugItem->PreRender();
		if (FLightManager::GetInstance().GetDirectLights().size() > 0)
		{
			m_ShadowMapDebugItem->m_Material->SetTexture2D("gShadowMap", m_ShadowMapRenderTargets[0]->GetTexture(EAttachmentIndex::DepthStencil));
		}
		m_ShadowMapDebugItem->Render();
		m_ShadowMapDebugRenderTarget->UnBind();
	}

	void FShadowStage::RenderShadowMap()
	{
		const auto& DirectLights = FLightManager::GetInstance().GetDirectLights();
		for (int32_t Index = 0; Index < DirectLights.size(); ++Index)
		{
			UDirectLightActor* DirectLight = DirectLights[Index];
			m_ShadowMapRenderTargets[Index]->Bind();
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_SHADOW);
			UDirectLightComponnet*  DirectLightComponent = DirectLight->GetComponent<UDirectLightComponnet>();
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->PreRender();
				/*
				RenderItem->m_Material->SetCameraProjectViewMat("View", DirectLightComponent->GetView());
				RenderItem->m_Material->SetCameraProjectViewMat("Projection", DirectLightComponent->GetProject());
				RenderItem->m_Material->SetCameraProjectViewMat("ProjectionView", DirectLightComponent->GetProjectView());
				*/
				RenderItem->Render();
			}
			m_ShadowMapRenderTargets[Index]->UnBind();
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
		RenderShadowMapDebug();
	}
}
