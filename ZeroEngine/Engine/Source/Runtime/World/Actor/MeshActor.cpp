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
		m_MeshVertexComponent = CreateComponent<UMeshVertexComponent>(this);
		m_MeshRenderComponent = CreateComponent<UMeshRenderComponent>(this);
	}

	void UMeshActor::PostInit()
	{
		Supper::PostInit();
		BuildMesh();
		m_MeshVertexComponent->GenerateAABB();

		uint32_t SubMeshNum = m_MeshVertexComponent->m_Mesh->GetSubMeshNum();
		m_MeshRenderComponent->SetSubmeshNum(SubMeshNum);
		m_MeshRenderComponent->AttachParameters();
	}

	void UMeshActor::CommitToPipieline()
	{
		FRenderItemPool& RenderItemPool = m_World->GetRenderItemPool(RENDERLAYER_OPAQUE);
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FRenderItem> Item = RenderItemPool.Request();
			Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
			Item->m_SubMesh = SubMesh;
			Item->m_ConstantsBuffer = m_MeshVertexComponent->m_ShaderConstantsBuffer;
			Item->m_Material = m_MeshRenderComponent->GetPassMaterials(RENDERLAYER_OPAQUE)[MaterialIndex];
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