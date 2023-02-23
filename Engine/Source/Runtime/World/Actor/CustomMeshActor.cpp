#include "CustomMeshActor.h"
#include "../Component/MeshVertexComponent.h"
#include "Render/Moudule/MeshGenerator.h"
#include "Utils/ZConfig.h"
#include "Render/RHI/Buffer/VertexBuffer.h"
#include "../Component/MeshRenderComponent.h"

namespace Zero
{
	UCustomMeshActor::UCustomMeshActor()
		: UMeshActor()
	{
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE, PSO_FORWARDLIT);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_SHADOW, PSO_SHADOWMAP);
	}
	UCustomMeshActor::UCustomMeshActor(const std::string& FileName)
		: UMeshActor()
		, m_FileName(FileName)
	{
		std::filesystem::path Path = m_FileName;
		SetTagName(Path.stem().string());
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE, PSO_FORWARDLIT);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_SHADOW, PSO_SHADOWMAP);
	}

	void UCustomMeshActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::Get().CreateCustomModel(MeshDatas, ZConfig::GetAssestsFullPath(m_FileName).string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
}
