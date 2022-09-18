#include "CubeMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"

namespace Zero
{
	UCubeMeshActor::UCubeMeshActor(const std::string& Tag)
		: UMeshActor(Tag, FMeshType::s_CubeMeshType)
	{
	}

	void UCubeMeshActor::BuildMesh()
	{
		FMeshData& MeshData =  m_MeshVertexComponent->GetMeshData();
		FMeshCreator::GetInstance().CreateCube(MeshData, m_Width, m_Height, m_Depth, 0);

		m_MeshVertexComponent->CreateMesh();
	}
}
