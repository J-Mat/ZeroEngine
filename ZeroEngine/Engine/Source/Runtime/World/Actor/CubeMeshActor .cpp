#include "MeshActor.h"
#include "Render/RendererAPI.h"
#include "../Component/MeshRenderComponent.h"
#include "../Component/MeshVertexComponent.h"
#include "../Construction/ObjectConstruction.h"
#include "Render/RHI/RenderItem.h"
#include "Render/RHI/Mesh.h"

namespace Zero
{
	UMeshActor::UMeshActor(const std::string& Tag)
		: UActor(Tag)
	{
	}

	void UMeshActor::PostInit()
	{
		m_MeshVertexComponent = CreateObject<UMeshVertexComponent>(GetWorld());
		m_MeshRenderComponent = CreateObject<UMeshRenderComponent>(GetWorld());
	}

	void UMeshActor::CommitToPipieline(FRenderItemPool& RenderItemPool)
	{
		uint32_t MaterialIndex = 0;
		for (FSubMesh& SubMesh : *m_MeshVertexComponent->m_Mesh.get())
		{
			Ref<FRenderItem> Item = RenderItemPool.Request();
			Item->m_Mesh = m_MeshVertexComponent->m_Mesh;
			Item->m_SubMesh = SubMesh.shared_from_this();
			Item->m_ConstantsBuffer = m_MeshVertexComponent->m_ShaderConstantsBuffer;
			Item->m_Material = m_MeshRenderComponent->m_Materials[EMeshRenderLayerType::RenderLayer_Opaque][MaterialIndex];
			MaterialIndex++;

			RenderItemPool.Push(Item);
		}
	}

}