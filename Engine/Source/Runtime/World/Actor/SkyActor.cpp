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
		: UMeshActor()
	{
		m_bVisibleInEditor = false;
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Skybox);
		m_MeshRenderComponent->SetEnableMaterial(false);
	}

	void USkyActor::BuildMesh()
	{
		std::vector<FMeshData>& MeshDatas = m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::Get().CreateCube(MeshData, 1.0f, 1.0f, 1.0f, 0);
		MeshDatas.push_back(MeshData);
		m_MeshVertexComponent->CreateMesh();
	}


	void USkyActor::SetCustomParemeters()
	{
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FMaterial> Material = m_MeshRenderComponent->GetPassMaterials(ERenderLayer::Skybox)[MaterialIndex];
			Material->SetTextureCubemap(m_TextureCubeMapName, m_TextureCubeMap.get());
			MaterialIndex++;
		}
	}

	void USkyActor::SetTextureCubemap(const std::string& TextureName, Ref<FTextureCube> TextureCubeMap)
	{
		m_TextureCubeMapName = TextureName;
		m_TextureCubeMap = TextureCubeMap;
	}
}