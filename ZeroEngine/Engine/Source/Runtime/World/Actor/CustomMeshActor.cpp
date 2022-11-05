#include "CustomMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "Utils/ZConfig.h"
#include "Render/RHI/VertexBuffer.h"
#include "../Component/MeshRenderComponent.h"

namespace Zero
{
	UCustomMeshActor::UCustomMeshActor(const std::string& FileName)
		: UMeshActor()
		, m_FileName(FileName)
	{
		GetComponent<UMeshRenderComponent>()->SetShader("ForwardLit.hlsl");
	}

	void UCustomMeshActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::GetInstance().CreateCustomModel(MeshDatas, ZConfig::GetObjFullPath(m_FileName).string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
}
