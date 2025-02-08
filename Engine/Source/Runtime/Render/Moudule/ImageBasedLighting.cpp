#include "ImageBasedLighting.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/ShaderParamsSettings.h"
#include "Render/RHI/GraphicPipelineStateObject.h"
#include "Render/RendererAPI.h"
#include "Render/Moudule/ConstantsBufferManager.h"
#include "Render/Moudule/PSOCache.h"
#include "MeshGenerator.h"
#include "Render/Moudule/Texture/TextureManager.h"
#include "Render/RenderUtils.h"



namespace Zero
{
	void FImageBasedLighting::InitIrradianceMap()
	{
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		//FMeshGenerator::Get().CreateCube(MeshData, 2.0f, 2.0f, 2.0f, 0);
		FMeshGenerator::Get().CreateSphere(MeshData, 1.0f, 3);
		//FMeshGenerator::GetInstance().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);

		m_IrradianceMapRenderItem->m_Mesh = FGraphic::GetDevice()->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
		m_IrradianceMapRenderItem->m_SubMesh = *m_IrradianceMapRenderItem->m_Mesh->begin();
		m_IrradianceMapRenderItem->m_ShaderParamsSettings = CreateRef<FShaderParamsSettings>();
		m_IrradianceMapRenderItem->m_PsoID = EGraphicPsoID::IBLIrradiance;
		m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetShader(m_IrradianceMapRenderItem->GetPSObj()->GetPSODescriptor().Shader);
		m_IrradianceMapRenderItem->m_PerObjectBuffer = FConstantsBufferManager::Get().GetPerObjectConstantsBuffer();
		m_IrradianceMapRenderItem->m_PerObjectBuffer->PreDrawCall();
		m_IrradianceMapRenderItem->SetModelMatrix(ZMath::mat4(1.0f));

		FRenderTargetCubeDesc IrradianceMapRTCubeDesc =
		{
			.RenderTargetName = "IrradianceMap",
			.Size = 256,
		};
		m_IBLIrradianceMapRTCube = FGraphic::GetDevice()->CreateRenderTargetCube(IrradianceMapRTCubeDesc);
	}

	void FImageBasedLighting::InitPrefilerMap()
	{
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::Get().CreateCube(MeshData, 1.0f, 1.0f, 1.0f, 0);
		//FMeshGenerator::GetInstance().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);
		Ref<FMesh> Mesh = FGraphic::GetDevice()->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout);

		for (uint32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			m_PrefilterMapRenderItems[Mip]->m_Mesh = Mesh;
			m_PrefilterMapRenderItems[Mip]->m_SubMesh = *m_PrefilterMapRenderItems[Mip]->m_Mesh->begin();
			m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings = CreateRef<FShaderParamsSettings>();
			m_PrefilterMapRenderItems[Mip]->m_PsoID = EGraphicPsoID::IBLPrefilter;
			m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetShader(m_PrefilterMapRenderItems[Mip]->GetPSObj()->GetPSODescriptor().Shader);
			m_PrefilterMapRenderItems[Mip]->m_PerObjectBuffer = FConstantsBufferManager::Get().GetPerObjectConstantsBuffer();
			m_PrefilterMapRenderItems[Mip]->m_PerObjectBuffer->PreDrawCall();
			m_PrefilterMapRenderItems[Mip]->SetModelMatrix(ZMath::mat4(1.0f));
		}

		uint32_t PrefilterMapSize = 128;
		FRenderTargetCubeDesc PrefilterMapRTCubeDesc =
		{
			.RenderTargetName = std::format("PrefilterMap"),
			.Size = PrefilterMapSize,
			.MipLevels = 5
		};
		m_PrefilterEnvMapRTCube = FGraphic::GetDevice()->CreateRenderTargetCube(PrefilterMapRTCubeDesc);
	}

	FImageBasedLighting::FImageBasedLighting(Ref<FRenderItem> IrradianceMapRenderItem, Ref<FRenderItem> PrefilterMapRenderItems[5])
		: m_IrradianceMapRenderItem(IrradianceMapRenderItem)
	{
		for (uint32_t i = 0; i < 5; ++i)
		{
			m_PrefilterMapRenderItems[i] = PrefilterMapRenderItems[i];
		}
		InitIrradianceMap();
		InitPrefilerMap();
	}

	void FImageBasedLighting::PreCaculate()
	{
		m_CommandListHandle = FDX12Device::Get()->GetSingleThreadCommadList();
		CreateIBLIrradianceMap();
		CreateIBLPrefilterEnvMap();
	}

	void FImageBasedLighting::CreateIBLIrradianceMap()
	{
		m_IBLIrradianceMapRTCube->Bind(m_CommandListHandle);
		auto TextureCubmap = FTextureManager::Get().GetCurrentSkybox();
		for (uint32_t FaceIndex = 0; FaceIndex < 6;++FaceIndex)
		{ 
			const FSceneView& SceneView = m_IBLIrradianceMapRTCube->GetSceneView(FaceIndex);
			Ref<IShaderConstantsBuffer> Camera = m_IBLIrradianceMapRTCube->GetCamera(FaceIndex);

			m_IBLIrradianceMapRTCube->SetRenderTarget(m_CommandListHandle, FaceIndex);


			{
				FRenderParams RenderSettings =
				{
					.RenderLayer = ERenderLayer::Unknown,
					.PiplineStateMode = EPiplineStateMode::AllSpecific,
					.PsoID = EGraphicPsoID::IBLIrradiance,
				};
				FRenderUtils::DrawRenderItem(m_IrradianceMapRenderItem, RenderSettings, m_CommandListHandle,
					[&](Ref<FRenderItem> RenderItem)
					{
						m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetCamera(Camera);
						m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetCameraViewMat("View", SceneView.View);
						m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetCameraViewPos("ViewPos", SceneView.ViewPos);
						m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetCameraProjectMat("Projection", SceneView.Proj);
						m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
						m_IrradianceMapRenderItem->m_ShaderParamsSettings->SetTextureCube("gSkyboxMap", TextureCubmap.get());
					});
			}
		}
		m_IBLIrradianceMapRTCube->UnBind(m_CommandListHandle, 0);
	}

	void FImageBasedLighting::CreateIBLPrefilterEnvMap()
	{
		m_PrefilterEnvMapRTCube->Bind(m_CommandListHandle);
		for (uint32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			float Roughness = (float)Mip / (float)(m_Mips - 1);
			auto TextureCubmap = FTextureManager::Get().GetCurrentSkybox();
			for (uint32_t FaceIndex = 0; FaceIndex < 6;++FaceIndex)
			{ 
				m_PrefilterEnvMapRTCube->SetRenderTarget(m_CommandListHandle, FaceIndex, Mip);
				const FSceneView& SceneView = m_PrefilterEnvMapRTCube->GetSceneView(FaceIndex);
				Ref<IShaderConstantsBuffer> Camera = m_PrefilterEnvMapRTCube->GetCamera(FaceIndex);

				FRenderParams RenderSettings =
				{
					.RenderLayer = ERenderLayer::Unknown,
					.PiplineStateMode = EPiplineStateMode::AllSpecific,
					.PsoID = EGraphicPsoID::IBLPrefilter
				};
				FRenderUtils::DrawRenderItem(m_PrefilterMapRenderItems[Mip], RenderSettings, m_CommandListHandle,
					[&](Ref<FRenderItem> RenderItem)
					{
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetCamera(Camera);
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetCameraViewMat("View", SceneView.View);
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetFloat("Roughness", Roughness);
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetCameraViewPos("ViewPos", SceneView.ViewPos);
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetCameraProjectMat("Projection", SceneView.Proj);
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
						m_PrefilterMapRenderItems[Mip]->m_ShaderParamsSettings->SetTextureCube("gSkyboxMap", TextureCubmap.get());
					}
				);
			}
		}
		m_PrefilterEnvMapRTCube->UnBind(m_CommandListHandle, 0);
	}

}
