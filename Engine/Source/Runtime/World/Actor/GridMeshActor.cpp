#include "GridMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "../Component/MeshRenderComponent.h"

namespace Zero
{
	UGridMeshActor::UGridMeshActor()
		: UMeshActor()
	{
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE, PSO_FORWARDLIT);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_SHADOW, PSO_SHADOWMAP);
	}

	UGridMeshActor::UGridMeshActor(float Width, float Height, uint32_t Rows, uint32_t Cols)
		: UMeshActor()
		, m_Width(Width)
		, m_Height(Height)
		, m_Rows(Rows)
		, m_Cols(Cols)
	{
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE, PSO_FORWARDLIT);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_SHADOW, PSO_SHADOWMAP);
	}

	void UGridMeshActor::BuildMesh()
	{
		std::vector<FMeshData>& MeshDatas = m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateGrid(MeshData, m_Width, m_Height, m_Rows, m_Cols);
		MeshDatas.push_back(MeshData);

		m_MeshVertexComponent->CreateMesh();
	}
}
