#include "CustomMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "Utils/ZConfig.h"
#include "Render/RHI/VertexBuffer.h"
#include "../Component/MeshRenderComponent.h"

namespace Zero
{
	UCustomMeshActor::UCustomMeshActor()
		: UMeshActor()
	{
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE);
	}
	UCustomMeshActor::UCustomMeshActor(const std::string& FileName)
		: UMeshActor()
		, m_FileName(FileName)
	{
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE);
	}

	void UCustomMeshActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::GetInstance().CreateCustomModel(MeshDatas, ZConfig::GetAssestsFullPath(m_FileName).string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
}
