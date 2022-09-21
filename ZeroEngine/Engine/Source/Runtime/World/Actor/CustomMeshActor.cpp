#include "CustomMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "Core/Config.h"
#include "Render/RHI/VertexBuffer.h"

namespace Zero
{
	UCustomMeshActor::UCustomMeshActor(const std::string& Tag, const std::string& FileName)
		: UMeshActor(Tag)
		, m_FileName(FConfig::GetInstance().GetObjFullPath(FileName).string())
	{
	}

	void UCustomMeshActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::GetInstance().CreateCustomModel(MeshDatas, m_FileName, FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
}
