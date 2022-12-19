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
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{
	void FForwardStage::InitIBLRenderCube()
	{
		Ref<FRenderItemPool> RenderItemPool = UWorld::GetCurrentWorld()->GetDIYRenderItemPool();
		m_IBLCubeRenderItem = RenderItemPool->Request();
		
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateCube(MeshData, 1.0f, 1.0f, 1.0f, 0);
		MeshDatas.push_back(MeshData);

		m_IBLCubeRenderItem->m_Mesh = FRenderer::GraphicFactroy->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
		m_IBLCubeRenderItem->m_SubMesh = *m_IBLCubeRenderItem->m_Mesh->begin();
		m_IBLCubeRenderItem->m_MaterialBuffer = nullptr;
		m_IBLCubeRenderItem->m_Material = CreateRef<FMaterial>(false);
		m_IBLCubeRenderItem->m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch("Shader\\IBLIrradiance.hlsl");
		m_IBLCubeRenderItem->m_Material->SetShader(m_IBLCubeRenderItem->m_PipelineStateObject->GetPSODescriptor().Shader);
		m_IBLCubeRenderItem->m_MaterialBuffer = FConstantsBufferManager::GetInstance().GetPerObjectConstantsBuffer();
		m_IBLCubeRenderItem->SetModelMatrix(ZMath::mat4(1.0f));
	}

	void FForwardStage::CreateIBLIrradianceMap()
	{
		m_RenderTargetCube->Bind();
		for (uint32_t i = 0; i < 6;++i)
		{ 
			const FSceneView& SceneView = m_RenderTargetCube->GetSceneView(i);
			m_RenderTargetCube->SetRenderTarget(i);
			m_IBLCubeRenderItem->m_PipelineStateObject->Bind();
			m_IBLCubeRenderItem->m_Material->SetPass();
			m_IBLCubeRenderItem->m_Material->SetCameraViewMat("View", SceneView.View);
			m_IBLCubeRenderItem->m_Material->SetCameraViewPos("ViewPos", SceneView.ViewPos);
			m_IBLCubeRenderItem->m_Material->SetCameraProjectMat("Projection", SceneView.Proj);
			m_IBLCubeRenderItem->m_Material->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
			m_IBLCubeRenderItem->m_Material->Tick();
			m_IBLCubeRenderItem->m_Material->OnDrawCall();
			m_IBLCubeRenderItem->OnDrawCall();
		}
		m_RenderTargetCube->UnBind();
	}

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

		FRenderTargetCubeDesc RenderTargetCubeDesc =
		{
			.Size = 256,
		};
		m_RenderTargetCube = FRenderer::GraphicFactroy->CreateRenderTargetCube(RenderTargetCubeDesc);
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
			m_RenderTarget->Bind();
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

			if (!m_bGenerateIrradianceMap)
			{
				InitIBLRenderCube();
				CreateIBLIrradianceMap();
				m_bGenerateIrradianceMap = true;
			}
		}

		m_RenderTarget->Bind();
		{
			auto RenderItemPool = UWorld::GetCurrentWorld()->GetRenderItemPool(RENDERLAYER_OPAQUE);
			for (Ref<FRenderItem> RenderItem : *RenderItemPool.get())
			{
				RenderItem->m_PipelineStateObject->Bind();
				if (m_bGenerateIrradianceMap)
				{
					RenderItem->m_Material->SetTextureCubemap("IBLIrradianceMap", m_RenderTargetCube->GetColorCubemap());
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
