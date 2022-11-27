#include "SkyActor.h"
#include "Render/RendererAPI.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/RHI/RenderItem.h"
#include "Render/RHI/Mesh.h"
#include "World/World.h"
#include "SkyActor.h"
#include "Render/Moudule/Material.h"
#include "Render/RHI/Texture.h"

namespace Zero
{
	USkyActor::USkyActor()
		: UActor()
	{
		m_MeshVertexComponent = CreateComponent<UMeshVertexComponent>(this);
		m_MeshRenderComponent = CreateComponent<UMeshRenderComponent>(this);
		m_MeshRenderComponent->SetShader("Shader/Skybox.hlsl");
		m_MeshRenderComponent->SetEnableMaterial(false);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_SKYBOX);
	}

	void USkyActor::BuildMesh()
	{
		std::vector<FMeshData>& MeshDatas = m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateCube(MeshData, 1.0f, 1.0f, 1.0f, 0);
		MeshDatas.push_back(MeshData);
		m_MeshVertexComponent->CreateMesh();
	}


	void USkyActor::PostInit()
	{
		Supper::PostInit();
		BuildMesh();

		uint32_t SubMeshNum = m_MeshVertexComponent->m_Mesh->GetSubMeshNum();
		m_MeshRenderComponent->SetSubmeshNum(SubMeshNum);
		m_MeshRenderComponent->GetPassMaterials(RENDERLAYER_SKYBOX);
		m_MeshRenderComponent->AttachParameters();
	}

	void USkyActor::CommitToPipieline()
	{
		FRenderItemPool& RenderItemPool = m_World->GetRenderItemPool(RENDERLAYER_SKYBOX);
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FRenderItem> Item = RenderItemPool.Request();
			Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
			Item->m_SubMesh = SubMesh;
			Item->m_ConstantsBuffer = m_MeshVertexComponent->m_ShaderConstantsBuffer;
			Item->m_Material = m_MeshRenderComponent->GetPassMaterials(RENDERLAYER_SKYBOX)[MaterialIndex];
			Item->SetModelMatrix(m_TransformationComponent->GetTransform());
			MaterialIndex++;
			RenderItemPool.Push(Item);
		}
		RenderItemPool.AddGuid(m_GUID);
	}

	void USkyActor::Tick()
	{
		CommitToPipieline();
	}

	void USkyActor::SetTextureCubemap(const std::string& TextureName, Ref<FTextureCubemap> TextureCubeMap)
	{
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FMaterial> Material = m_MeshRenderComponent->GetPassMaterials(RENDERLAYER_SKYBOX)[MaterialIndex];
			Material->SetTextureCubemap(TextureName, TextureCubeMap);
			MaterialIndex++;
		}
	}
}