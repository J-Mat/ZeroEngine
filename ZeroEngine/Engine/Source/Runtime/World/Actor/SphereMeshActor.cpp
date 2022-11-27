#include "SphereMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "../Component/MeshRenderComponent.h"

namespace Zero
{
	USphereMeshActor::USphereMeshActor()
		: UMeshActor()
	{
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE);
	}

	USphereMeshActor::USphereMeshActor(float Radius, uint32_t NumSubdivisions)
		: UMeshActor()
		, m_Radius(Radius)
		, m_NumSubdivisions(NumSubdivisions)
	{
	}
	

	void USphereMeshActor::BuildMesh()
	{
		std::vector<FMeshData>& MeshDatas = m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateSphere(MeshData, m_Radius, m_NumSubdivisions);
		MeshDatas.push_back(MeshData);

		m_MeshVertexComponent->CreateMesh();
	}
}
