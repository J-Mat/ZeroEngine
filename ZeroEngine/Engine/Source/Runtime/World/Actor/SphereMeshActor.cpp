#include "SphereMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"

namespace Zero
{
	USphereMeshActor::USphereMeshActor(const std::string& Tag)
		: UMeshActor(Tag, FMeshType::s_CubeMeshType)
	{
	}

	void USphereMeshActor::BuildMesh()
	{
		std::vector<FMeshData>& MeshDatas = m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateSphere(MeshData, m_Radius, 3);
		MeshDatas.push_back(MeshData);

		m_MeshVertexComponent->CreateMesh();
	}
}
