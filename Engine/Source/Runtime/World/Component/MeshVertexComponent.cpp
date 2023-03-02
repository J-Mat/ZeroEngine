#include "MeshVertexComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"

namespace Zero
{ 
	UMeshVertexComponent::UMeshVertexComponent()
		: UComponent()
	{
	}

	UMeshVertexComponent::~UMeshVertexComponent()
	{
	}

	void UMeshVertexComponent::CreateMesh()
	{
		m_Mesh = FRenderer::GetDevice()->CreateMesh(
			m_MeshDatas,
			FVertexBufferLayout::s_DefaultVertexLayout
		);
	}
	void UMeshVertexComponent::GenerateAABB()
	{
		for (FMeshData& MeshData : m_MeshDatas)
		{
			m_Mesh->Merge(MeshData.AABB);
		}
	}
}
