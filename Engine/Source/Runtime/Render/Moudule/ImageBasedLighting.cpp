#include "ImageBasedLighting.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/PipelineStateObject.h"
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
		m_IrradianceMapRenderItem->m_Material = CreateRef<FMaterial>(false);
		m_IrradianceMapRenderItem->m_PsoID = EPsoID::IBLIrradiance;
		m_IrradianceMapRenderItem->m_Material->SetShader(m_IrradianceMapRenderItem->GetPsoObj()->GetPSODescriptor().Shader);
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

		for (int32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			m_PrefilterMapRenderItem[Mip]->m_Mesh = Mesh;
			m_PrefilterMapRenderItem[Mip]->m_SubMesh = *m_PrefilterMapRenderItem[Mip]->m_Mesh->begin();
			m_PrefilterMapRenderItem[Mip]->m_Material = CreateRef<FMaterial>(false);
			m_PrefilterMapRenderItem[Mip]->m_PsoID = EPsoID::IBLPrefilter;
			m_PrefilterMapRenderItem[Mip]->m_Material->SetShader(m_PrefilterMapRenderItem[Mip]->GetPsoObj()->GetPSODescriptor().Shader);
			m_PrefilterMapRenderItem[Mip]->m_PerObjectBuffer = FConstantsBufferManager::Get().GetPerObjectConstantsBuffer();
			m_PrefilterMapRenderItem[Mip]->m_PerObjectBuffer->PreDrawCall();
			m_PrefilterMapRenderItem[Mip]->SetModelMatrix(ZMath::mat4(1.0f));
		}
		uint32_t PrefilterMapSize = 128;
		m_PrefilterEnvMapRTCubes.resize(m_Mips);
		m_PrefilterEnvMapTextureCubes.resize(m_Mips);

		for (int32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			FRenderTargetCubeDesc PrefilterMapRTCubeDesc =
			{
				.RenderTargetName = std::format("PrefilterMap_Mip_{0}", Mip),
				.Size = PrefilterMapSize,
				.MipLevels = 1
			};
			m_PrefilterEnvMapRTCubes[Mip] = FGraphic::GetDevice()->CreateRenderTargetCube(PrefilterMapRTCubeDesc);
			m_PrefilterEnvMapTextureCubes[Mip] = m_PrefilterEnvMapRTCubes[Mip]->GetColorCubemap();
			PrefilterMapSize /= 2;
		}
	}

	FImageBasedLighting::FImageBasedLighting(Ref<FRenderItem> IrradianceMapRenderItem, Ref<FRenderItem> PrefilterMapRenderItem[5])
		: m_IrradianceMapRenderItem(IrradianceMapRenderItem)
	{
		for (int i = 0; i < 5; ++i)
		{
			m_PrefilterMapRenderItem[i] = PrefilterMapRenderItem[i];
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
		for (uint32_t i = 0; i < 6;++i)
		{ 
			const FSceneView& SceneView = m_IBLIrradianceMapRTCube->GetSceneView(i);
			Ref<IShaderConstantsBuffer> Camera = m_IBLIrradianceMapRTCube->GetCamera(i);
			m_IBLIrradianceMapRTCube->SetRenderTarget(i, m_CommandListHandle);
			m_IrradianceMapRenderItem->m_Material->SetCamera(Camera);

			FRenderUtils::DrawRenderItem(m_IrradianceMapRenderItem, m_CommandListHandle, 
				[&](Ref<FRenderItem> RenderItem)
				{
					m_IrradianceMapRenderItem->m_Material->SetCameraViewMat("View", SceneView.View);
					m_IrradianceMapRenderItem->m_Material->SetCameraViewPos("ViewPos", SceneView.ViewPos);
					m_IrradianceMapRenderItem->m_Material->SetCameraProjectMat("Projection", SceneView.Proj);
					m_IrradianceMapRenderItem->m_Material->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
					m_IrradianceMapRenderItem->m_Material->SetTextureCubemap("gSkyboxMap", TextureCubmap);
				});
		}
		m_IBLIrradianceMapRTCube->UnBind(m_CommandListHandle);
	}

	void FImageBasedLighting::CreateIBLPrefilterEnvMap()
	{
		for (int32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			float Roughness = (float)Mip / (float)(m_Mips - 1);
			m_PrefilterEnvMapRTCubes[Mip]->Bind(m_CommandListHandle);
			auto TextureCubmap = FTextureManager::Get().GetCurrentSkybox();
			for (uint32_t i = 0; i < 6;++i)
			{ 
				const FSceneView& SceneView = m_PrefilterEnvMapRTCubes[Mip]->GetSceneView(i);
				Ref<IShaderConstantsBuffer> Camera = m_PrefilterEnvMapRTCubes[Mip]->GetCamera(i);
				m_PrefilterMapRenderItem[Mip]->m_Material->SetCamera(Camera);

				m_PrefilterEnvMapRTCubes[Mip]->SetRenderTarget(i, m_CommandListHandle);
				FRenderUtils::DrawRenderItem(m_PrefilterMapRenderItem[Mip], m_CommandListHandle,
					[&](Ref<FRenderItem> RenderItem)
					{
						m_PrefilterMapRenderItem[Mip]->m_Material->SetCameraViewMat("View", SceneView.View);
						m_PrefilterMapRenderItem[Mip]->m_Material->SetFloat("Roughness", Roughness);
						m_PrefilterMapRenderItem[Mip]->m_Material->SetCameraViewPos("ViewPos", SceneView.ViewPos);
						m_PrefilterMapRenderItem[Mip]->m_Material->SetCameraProjectMat("Projection", SceneView.Proj);
						m_PrefilterMapRenderItem[Mip]->m_Material->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
						m_PrefilterMapRenderItem[Mip]->m_Material->SetTextureCubemap("gSkyboxMap", TextureCubmap);
					}
				);
			}
			m_PrefilterEnvMapRTCubes[Mip]->UnBind(m_CommandListHandle);
		}
	}

}
