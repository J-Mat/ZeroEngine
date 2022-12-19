#include "MeshVertexComponent.h"
#include "Render/RendererAPI.h"
#include "World/World.h"
#include "Render/Moudule/ConstantsBufferManager.h"

namespace Zero
{ 
	UMeshVertexComponent::UMeshVertexComponent()
		: UComponent()
	{
		m_PerObjConstantsBuffer = FConstantsBufferManager::GetInstance().GetPerObjectConstantsBuffer();
	}

	UMeshVertexComponent::~UMeshVertexComponent()
	{
		FConstantsBufferManager::GetInstance().PushPerObjectConstantsBufferToPool(m_PerObjConstantsBuffer);
	}

	void UMeshVertexComponent::CreateMesh()
	{
		m_Mesh = FRenderer::GraphicFactroy->CreateMesh(
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
