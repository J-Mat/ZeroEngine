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
		std::vector<FMeshData>& MeshDatas =  m_MeshVertexComponent->GetMeshData();
		FMeshData MeshData;
		FMeshCreator::GetInstance().CreateCube(MeshData, m_Width, m_Height, m_Depth, 0);
		MeshDatas.push_back(MeshData);

		m_MeshVertexComponent->CreateMesh();
	}
}
