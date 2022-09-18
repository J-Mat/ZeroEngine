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
		FMeshData& MeshData = m_MeshVertexComponent->GetMeshData();
		FMeshCreator::GetInstance().CreateSphere(MeshData, m_Radius, 3);

		m_MeshVertexComponent->CreateMesh();
	}
}
