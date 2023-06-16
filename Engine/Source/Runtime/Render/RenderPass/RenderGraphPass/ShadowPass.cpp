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
#include "Render/RHI/ComputePipelineStateObject.h"



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
			m_ShadowMapDebugItems[LightIndex]->m_PsoID = EGraphicPsoID::ShadowDebug;
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
					Builder.DeclareTexture2D(RGResourceName::DirectLightShadowMaps[LightIndex], FRGTextureDesc::MakeShadow2DDesc(m_Width, m_Height));
					Builder.WriteDepthStencil2D(RGResourceName::DirectLightShadowMaps[LightIndex], ERGLoadStoreAccessOp::Clear_Preserve);
					Builder.SetViewport(m_Width, m_Height);
				},
				[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
				{
					{
						FRenderParams ShadowRenderSettings =
						{
							.RenderLayer = ERenderLayer::Shadow,
							.PiplineStateMode = EPiplineStateMode::AllSpecific,
							.PsoID = EGraphicPsoID::DirectLightShadowMap,
						};
						FRenderUtils::RenderLayer(ShadowRenderSettings, CommandListHandle,
							[=](Ref<FRenderItem> RenderItem)
							{
								RenderItem->m_Material->SetCamera(LightCamera);
							}
						);
					}
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
					{
						FRenderParams ShadowDebugRenderParams =
						{
							.RenderLayer = ERenderLayer::Unknown,
							.PiplineStateMode = EPiplineStateMode::AllSpecific,
							.PsoID = EGraphicPsoID::DirectLightShadowMap,
						};
						FRenderUtils::DrawRenderItem(m_ShadowMapDebugItems[LightIndex], ShadowDebugRenderParams, CommandListHandle,
							[&](Ref<FRenderItem> RenderItem)
							{
								RenderItem->m_Material->SetTexture2D("_gShadowMap", ShadowMap);
							}
						);
					}
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
				std::string PassName = std::format("PointLightShadowMap Pass {0}", FaceIndex);
				RenderGraph.AddPass<void>(
					PassName.c_str(),
					[=](FRenderGraphBuilder& Builder)
					{
						if (FaceIndex == 0)
						{ 
							Builder.DeclareTextureCube(RGResourceName::PointLightShadowMaps[LightIndex], FRGTextureDesc::MakeShadowCubeDesc(m_Width, m_Height));
						}
						Builder.WriteDepthStencilCube(RGResourceName::PointLightShadowMaps[LightIndex], ERGLoadStoreAccessOp::Clear_Preserve, 0, -1, FaceIndex);
						Builder.SetViewport(m_Width, m_Height, FaceIndex);
					},
					[=](FRenderGraphContext& Context, FCommandListHandle CommandListHandle)
					{
						UPointLightActor* PointLight = FLightManager::Get().GetPointLights()[LightIndex];
						const FSceneCaptureCube& SceneCaptureCube = PointLight->GetSceneCaptureCube();
						const FSceneView& SceneView = SceneCaptureCube.GetSceneView(FaceIndex); 
						const Ref<IShaderConstantsBuffer> Camera = SceneCaptureCube.GetCamera(FaceIndex);
						{
							FRenderParams ShadowRenderParams =
							{
								.RenderLayer = ERenderLayer::Shadow,
								.PiplineStateMode = EPiplineStateMode::AllSpecific,
								.PsoID = EGraphicPsoID::PointLightShadowMap,
							};
							FRenderUtils::RenderLayer(ShadowRenderParams, CommandListHandle,
								[=](Ref<FRenderItem> RenderItem)
								{
									RenderItem->m_Material->SetCamera(Camera);
								}
							);
						}
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
