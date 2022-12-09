#include "DirectLightActor.h"
#include "World/LightManager.h"


namespace Zero
{
	UDirectLightActor::UDirectLightActor()
		:UMeshActor()
	{
		m_LightComponnet = CreateComponent<UDirectLightComponnet>(this);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE);
		GetComponent<UMeshRenderComponent>()->SetEnableMaterial(false);
		GetComponent<UMeshRenderComponent>()->SetPsoType(EPsoType::PT_DirectLight);
	}

	void UDirectLightActor::PostInit()
	{
		Supper::PostInit();
		FLightManager::GetInstance().AddDirectLight(this);
	}

	void UDirectLightActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::GetInstance().CreateCustomModel(MeshDatas, ZConfig::GetAssestsFullPath("Obj/direct_light.fbx").string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
} 