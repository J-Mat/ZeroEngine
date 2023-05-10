#include "DirectLightActor.h"
#include "World/LightManager.h"
#include "ZConfig.h"


namespace Zero
{
	UDirectLightActor::UDirectLightActor()
		:UMeshActor()
	{
		m_LightComponnet = CreateComponent<UDirectLightComponnet>(this);
		m_MeshRenderComponent->AttachRenderLayer(ERenderLayer::Light);
		m_MeshRenderComponent->SetEnableMaterial(false);
	}

	void UDirectLightActor::PostInit()
	{
		Supper::PostInit();
		FLightManager::Get().AddDirectLight(this);
	}

	void UDirectLightActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::Get().CreateCustomModel(MeshDatas, ZConfig::GetAssestsFullPath("Obj/DirectLight.obj").string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
} 