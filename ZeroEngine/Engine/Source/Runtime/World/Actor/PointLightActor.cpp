#include "PointLightActor.h"
#include "World/LightManager.h"


namespace Zero
{
	UPointLightActor::UPointLightActor()
		:UMeshActor()
	{
		m_LightComponnet = CreateComponent<UPointLightComponnet>(this);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_OPAQUE);
		GetComponent<UMeshRenderComponent>()->SetEnableMaterial(false);
		GetComponent<UMeshRenderComponent>()->SetPsoType(EPsoType::PT_PointLight);
	}

	void UPointLightActor::PostInit()
	{
		Supper::PostInit();
		FLightManager::GetInstance().AddPointLight(this);
	}
	

	void UPointLightActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::GetInstance().CreateSphere(MeshData, 0.5f , 1);
		MeshDatas.push_back(MeshData);
		m_MeshVertexComponent->CreateMesh();
	}
} 