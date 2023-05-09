#include "ShadowPass.h"
#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderGraph/RenderGraphBuilder.h"
#include "Render/RenderGraph/RenderGraphContext.h"
#include "World/World.h"
#include "Render/RenderConfig.h"
#include "Render/RHI/Texture.h"
#include "Render/RHI/CommandList.h"
#include "Render/RenderUtils.h"
#include "Render/Moudule/Material.h"
#include "Render/Moudule/ImageBasedLighting.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "World/LightManager.h"
#include "Render/Moudule/PSOCache.h"
#include "Render/Moudule/SceneCapture.h"



namespace Zero
{
	FShadowPass::FShadowPass(uint32_t Width, uint32_t Height)
		: m_Width(Width)
		, m_Height(Height)
	{
		Ref<FRenderItemPool> RenderItemPool = UWorld::GetCurrentWorld()->GetDIYRenderItemPool();
			
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::Get().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);

		Ref<FMesh> Rect = FGraphic::GetDevice()->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
		for (uint32_t LightIndex = 0; LightIndex < FLightManager::Get().GetMaxDirectLightsNum(); ++LightIndex)
		{
			m_ShadowMapDebugItems[LightIndex] = RenderItemPool->Request();
			m_ShadowMapDebugItems[LightIndex]->m_Mesh = Rect;
			m_ShadowMapDebugItems[LightIndex]->m_SubMesh = *m_ShadowMapDebugItems[LightIndex]->m_Mesh->begin();
			m_ShadowMapDebugItems[LightIndex]->m_Material = CreateRef<FMaterial>();
			m_ShadowMapDebugItems[LightIndex]->m_PsoID = EPsoID::ShadowDebug;
			m_ShadowMapDebugItems[LightIndex]->m_Material->SetShader(m_ShadowMapDebugItems[LightIndex]->GetPsoObj()->GetPSODescriptor().Shader);
		}
	}

	void FShadowPass::SetupDirectLightShadow(FRenderGraph& RenderGraph)
	{
		const std::vector<UDirectLightActor*>& DirectLights = FLightManager::Get().GetDirectLights();
		for (uint32_t LightIndex = 0; LightIndex < DirectLights.size(); ++LightIndex)
		{
			FSceneCapture2D& SceneCapture2D = DirectLights[LightIndex]->GetSceneCapture2D();
			auto LightCamera = SceneCapture2D.GetCamera();
			auto& Pass = RenderGraph.AddPass<void>(
				"DirectLightShadowMap Pass",
				[=](FRenderGraphBuilder& Builder)
				{
					FRGTextureDesc ShadowMapDesc = {
						.Width = m_Width,
						.Height = m_Height,
						.ClearValue = FTextureClearValue(1.0f, 0),
						.Format = EResourceFormat::D24_UNORM_S8_UINT,
					};

					Builder.DeclareTexture2D(RGResourceName::DirectLightShadowMaps[LightIndex], ShadowMapDesc);
					Builder.WriteDepthStencil2D(RGResourceName::DirectLightShadowMaps[LightIndex], ERGLoadStoreAccessOp::Clear_Preserve);
					Builder.SetViewport(m_Width, m_Height);
				},
				[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
				{
					FRenderUtils::RenderLayer(ERenderLayer::Shadow, CommandListHandle,
					[=](Ref<FRenderItem> RenderItem)
						{
							RenderItem->m_Material->SetCamera(LightCamera);
						}
				);
				},
				ERenderPassType::Graphics,
				ERGPassFlags::ForceNoCull
			);


			struct FShadowPassDebugData
			{
				FRGTex2DReadOnlyID ShadowMapID;
			};
			RenderGraph.AddPass<FShadowPassDebugData>(
				"ShadowMap Debug Pass",
				[=](FShadowPassDebugData& Data, FRenderGraphBuilder& Builder)
				{
					Data.ShadowMapID = Builder.ReadTexture2D(RGResourceName::DirectLightShadowMaps[LightIndex]);
					Builder.WriteRenderTarget2D(RGResourceName::ShadowMaps_Debug[LightIndex], ERGLoadStoreAccessOp::Clear_Preserve);
					Builder.SetViewport(m_Width, m_Height);
				},
				[=](const FShadowPassDebugData& Data, FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
				{
					FTexture2D* ShadowMap = Context.GetTexture2D(Data.ShadowMapID.GetResourceID());
					FRenderUtils::DrawRenderItem(m_ShadowMapDebugItems[LightIndex], CommandListHandle,
						[&](Ref<FRenderItem> RenderItem)
						{
							RenderItem->m_Material->SetTexture2D("_gShadowMap", ShadowMap);
						}
					);
				},
				ERenderPassType::Graphics,
				ERGPassFlags::ForceNoCull
			);
		}
	}

	void FShadowPass::SetupPointLightShadow(FRenderGraph& RenderGraph)
	{
		const std::vector<UPointLightActor*>& PointLights = FLightManager::Get().GetPointLights();
		for (uint32_t LightIndex = 0; LightIndex < PointLights.size(); ++LightIndex)
		{
			for (uint32_t FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
			{
				RenderGraph.AddPass<void>(
					"PointLightShadowMap Pass",
					[=](FRenderGraphBuilder& Builder)
					{
						FRGTextureDesc ShadowMapDesc = {
							.Width = m_Width,
							.Height = m_Height,
							.ArraySize = 6,
							.ClearValue = FTextureClearValue(1.0f, 0),
							.Format = EResourceFormat::D24_UNORM_S8_UINT,
						};

						if (FaceIndex == 0)
						{ 
							Builder.DeclareTextureCube(RGResourceName::PointLightShadowMaps[LightIndex], ShadowMapDesc);
						}
						Builder.WriteDepthStencilCube(RGResourceName::PointLightShadowMaps[LightIndex], ERGLoadStoreAccessOp::Clear_Preserve);
						Builder.SetViewport(m_Width, m_Height, FaceIndex);
					},
					[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
					{
						FRenderUtils::RenderLayer(ERenderLayer::Shadow, CommandListHandle,
						[=](Ref<FRenderItem> RenderItem)
							{
								UPointLightActor* PointLight = FLightManager::Get().GetPointLights()[LightIndex];
								const FSceneCaptureCube& SceneCaptureCube = PointLight->GetSceneCaptureCube();
								const FSceneView& SceneView = SceneCaptureCube.GetSceneView(FaceIndex); 
								const Ref<IShaderConstantsBuffer> Camera = SceneCaptureCube.GetCamera(FaceIndex);
								RenderItem->m_Material->SetCamera(Camera);
								RenderItem->m_Material->SetCameraViewMat("View", SceneView.View);
								RenderItem->m_Material->SetCameraViewPos("ViewPos", SceneView.ViewPos);
								RenderItem->m_Material->SetCameraProjectMat("Projection", SceneView.Proj);
								RenderItem->m_Material->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
							}
						);
					},
					ERenderPassType::Graphics,
					ERGPassFlags::ForceNoCull
				);
			}
		}
	}

	void FShadowPass::AddPass(FRenderGraph& RenderGraph)
	{
		SetupDirectLightShadow(RenderGraph);
		SetupPointLightShadow(RenderGraph);
	}

	void FShadowPass::OnResize(uint32_t Width, uint32_t Height)
	{
		m_Width = Width;
		m_Height = Height;
	}
}
