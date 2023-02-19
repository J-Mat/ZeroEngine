#include "ImageBasedLighting.h"
#include "Render/RendererAPI.h"
#include "Render/RHI/RenderItem.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/PipelineStateObject.h"
#include "Render/Moudule/ConstantsBufferManager.h"
#include "MeshGenerator.h"


namespace Zero
{
	void FImageBasedLighting::InitIrradianceMap()
	{
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateCube(MeshData, 1.0f, 1.0f, 1.0f, 0);
		//FMeshGenerator::GetInstance().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);

		m_IrradianceMapRenderItem->m_Mesh = FRenderer::GraphicFactroy->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
		m_IrradianceMapRenderItem->m_SubMesh = *m_IrradianceMapRenderItem->m_Mesh->begin();
		m_IrradianceMapRenderItem->m_Material = CreateRef<FMaterial>(false);
		m_IrradianceMapRenderItem->m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch("Shader\\IBL\\IBLIrradiance.hlsl");
		m_IrradianceMapRenderItem->m_Material->SetShader(m_IrradianceMapRenderItem->m_PipelineStateObject->GetPSODescriptor().Shader);
		m_IrradianceMapRenderItem->m_PerObjectBuffer = FConstantsBufferManager::GetInstance().GetPerObjectConstantsBuffer();
		m_IrradianceMapRenderItem->SetModelMatrix(ZMath::mat4(1.0f));

		FRenderTargetCubeDesc IrradianceMapRTCubeDesc =
		{
			.RenderTargetName = "IrradianceMap",
			.Size = 256,
		};
		m_IBLIrradianceMapRTCube = FRenderer::GraphicFactroy->CreateRenderTargetCube(IrradianceMapRTCubeDesc);
	}

	void FImageBasedLighting::InitPrefilerMap()
	{
		std::vector<FMeshData> MeshDatas;
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateCube(MeshData, 1.0f, 1.0f, 1.0f, 0);
		//FMeshGenerator::GetInstance().CreateRect(MeshData);
		MeshDatas.push_back(MeshData);
		Ref<FMesh> Mesh = FRenderer::GraphicFactroy->CreateMesh(
			MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout);

		for (int32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			m_PrefilterMapRenderItem[Mip]->m_Mesh = Mesh;
			m_PrefilterMapRenderItem[Mip]->m_SubMesh = *m_PrefilterMapRenderItem[Mip]->m_Mesh->begin();
			m_PrefilterMapRenderItem[Mip]->m_Material = CreateRef<FMaterial>(false);
			m_PrefilterMapRenderItem[Mip]->m_PipelineStateObject = TLibrary<FPipelineStateObject>::Fetch("Shader\\IBL\\IBLPrefilter.hlsl");
			m_PrefilterMapRenderItem[Mip]->m_Material->SetShader(m_PrefilterMapRenderItem[Mip]->m_PipelineStateObject->GetPSODescriptor().Shader);
			m_PrefilterMapRenderItem[Mip]->m_PerObjectBuffer = FConstantsBufferManager::GetInstance().GetPerObjectConstantsBuffer();
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
			};
			m_PrefilterEnvMapRTCubes[Mip] = FRenderer::GraphicFactroy->CreateRenderTargetCube(PrefilterMapRTCubeDesc);
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
		m_CommandListHandle = FRenderer::GetDevice()->GetSingleThreadCommadList();
		CreateIBLIrradianceMap();
		CreateIBLPrefilterEnvMap();
	}


	void FImageBasedLighting::CreateIBLIrradianceMap()
	{
		m_IBLIrradianceMapRTCube->Bind(m_CommandListHandle);
		auto TextureCubmap = TLibrary<FTextureCubemap>::Fetch("default");
		for (uint32_t i = 0; i < 6;++i)
		{ 
			const FSceneView& SceneView = m_IBLIrradianceMapRTCube->GetSceneView(i);
			m_IBLIrradianceMapRTCube->SetRenderTarget(i, m_CommandListHandle);
			m_IrradianceMapRenderItem->PreRender(m_CommandListHandle);
			m_IrradianceMapRenderItem->m_Material->SetCameraViewMat("View", SceneView.View);
			m_IrradianceMapRenderItem->m_Material->SetCameraViewPos("ViewPos", SceneView.ViewPos);
			m_IrradianceMapRenderItem->m_Material->SetCameraProjectMat("Projection", SceneView.Proj);
			m_IrradianceMapRenderItem->m_Material->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
			m_IrradianceMapRenderItem->m_Material->SetTextureCubemap("gSkyboxMap", TextureCubmap);
			m_IrradianceMapRenderItem->Render(m_CommandListHandle);
		}
		m_IBLIrradianceMapRTCube->UnBind(m_CommandListHandle);
	}

	void FImageBasedLighting::CreateIBLPrefilterEnvMap()
	{
		for (int32_t Mip = 0; Mip < m_Mips; ++Mip)
		{
			float Roughness = (float)Mip / (float)(m_Mips - 1);
			m_PrefilterEnvMapRTCubes[Mip]->Bind(m_CommandListHandle);
			auto TextureCubmap = TLibrary<FTextureCubemap>::Fetch("default");
			for (uint32_t i = 0; i < 6;++i)
			{ 
				const FSceneView& SceneView = m_PrefilterEnvMapRTCubes[Mip]->GetSceneView(i);
				m_PrefilterEnvMapRTCubes[Mip]->SetRenderTarget(i, m_CommandListHandle);
				int32_t Mip_1 = Mip;
				m_PrefilterMapRenderItem[Mip_1]->PreRender(m_CommandListHandle);
				m_PrefilterMapRenderItem[Mip_1]->m_Material->SetCameraViewMat("View", SceneView.View);
				m_PrefilterMapRenderItem[Mip_1]->m_Material->SetFloat("Roughness", Roughness);
				m_PrefilterMapRenderItem[Mip_1]->m_Material->SetCameraViewPos("ViewPos", SceneView.ViewPos);
				m_PrefilterMapRenderItem[Mip_1]->m_Material->SetCameraProjectMat("Projection", SceneView.Proj);
				m_PrefilterMapRenderItem[Mip_1]->m_Material->SetCameraProjectViewMat("ProjectionView", SceneView.ProjectionView);
				m_PrefilterMapRenderItem[Mip_1]->m_Material->SetTextureCubemap("gSkyboxMap", TextureCubmap);
				m_PrefilterMapRenderItem[Mip_1]->Render(m_CommandListHandle);
			}
			m_PrefilterEnvMapRTCubes[Mip]->UnBind(m_CommandListHandle);
		}
	}

}
