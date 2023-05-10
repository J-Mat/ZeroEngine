#include "CustomMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "Utils/ZConfig.h"
#include "../Component/MeshRenderComponent.h"

namespace Zero
{
	UCustomMeshActor::UCustomMeshActor()
		: UMeshActor()
	{
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Opaque, EPsoID::ForwadLit);
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Shadow);
	}
	UCustomMeshActor::UCustomMeshActor(const std::string& FileName)
		: UMeshActor()
		, m_FileName(FileName)
	{
		std::filesystem::path Path = m_FileName;
		SetTagName(Path.stem().string());
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Opaque, EPsoID::ForwadLit);
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Shadow);
	}

	void UCustomMeshActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::Get().CreateCustomModel(MeshDatas, ZConfig::GetAssestsFullPath(m_FileName).string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
}
