#include "MeshActor.h"
#include "Render/RendererAPI.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"
#include "../Construction/ObjectConstruction.h"
#include "Render/RHI/RenderItem.h"
#include "Render/RHI/Mesh.h"

namespace Zero
{
	UMeshActor::UMeshActor(const std::string& Tag, FMeshType& MeshType)
		: UActor(Tag)
		, m_MeshType(MeshType)
	{
	}

	void UMeshActor::PostInit()
	{
		m_MeshVertexComponent = UComponent::CreateComponent<UMeshVertexComponent>(this, m_MeshType);
		m_MeshRenderComponent =  UComponent::CreateComponent<UMeshRenderComponent>(this);
		uint32_t SubMeshNum = m_MeshVertexComponent->m_Mesh->GetSubMeshNum();
		m_MeshRenderComponent->SetSubmeshNum(SubMeshNum);
	}

	void UMeshActor::CommitToPipieline(FRenderItemPool& RenderItemPool)
	{
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FRenderItem> Item = RenderItemPool.Request();
			Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
			Item->m_SubMesh = SubMesh;
			Item->m_ConstantsBuffer = m_MeshVertexComponent->m_ShaderConstantsBuffer;
			Item->m_Material = m_MeshRenderComponent->GetPassMaterials(EMeshRenderLayerType::RenderLayer_Opaque)[MaterialIndex];
			MaterialIndex++;

			RenderItemPool.Push(Item);
		}
	}

	void UMeshActor::Tick()
	{
	}

}