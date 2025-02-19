#include "CubeMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "../Component/MeshRenderComponent.h"
#include "Render/Moudule/MeshGenerator.h"

namespace Zero
{
	UCubeMeshActor::UCubeMeshActor()
		: UMeshActor()
	{
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Opaque, EGraphicPsoID::ForwadLit);
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Shadow);
	}

	void UCubeMeshActor::BuildMesh()
	{
		std::vector<FMeshData>& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::Get().CreateCube(MeshData, m_Width, m_Height, m_Depth, 0);
		MeshDatas.push_back(MeshData);

		m_MeshVertexComponent->CreateMesh();
	}
}
