#include "MeshActor.h"
#include "Render/RendererAPI.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/RHI/RenderItem.h"
#include "Render/RHI/Mesh.h"
#include "World/World.h"

namespace Zero
{
	UMeshActor::UMeshActor()
		: UActor()
	{
		CreateComponent<UMeshVertexComponent>(this);
		CreateComponent<UMeshRenderComponent>(this);
	}

	void UMeshActor::PostInit()
	{
		m_MeshVertexComponent = GetComponent<UMeshVertexComponent>();
		m_MeshRenderComponent = GetComponent<UMeshRenderComponent>();
		BuildMesh();
		m_MeshVertexComponent->GenerateAABB();

		uint32_t SubMeshNum = m_MeshVertexComponent->m_Mesh->GetSubMeshNum();
		m_MeshRenderComponent->SetSubmeshNum(SubMeshNum);
	}

	void UMeshActor::CommitToPipieline()
	{
		FRenderItemPool& RenderItemPool = m_World->GetRenderItemPool();
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FRenderItem> Item = RenderItemPool.Request();
			Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
			Item->m_SubMesh = SubMesh;
			Item->m_ConstantsBuffer = m_MeshVertexComponent->m_ShaderConstantsBuffer;
			Item->m_Material = m_MeshRenderComponent->GetPassMaterials(EMeshRenderLayerType::RenderLayer_Opaque)[MaterialIndex];
			Item->SetModelMatrix(m_TransformationComponent->GetTransform());
			MaterialIndex++;

			RenderItemPool.Push(Item);
		}
		RenderItemPool.AddGuid(m_GUID);
	}

	void UMeshActor::Tick()
	{
		CommitToPipieline();
	}

	ZMath::FAABB UMeshActor::GetAABB()
	{
		return m_MeshVertexComponent->m_Mesh->GetAABB();
	}

}