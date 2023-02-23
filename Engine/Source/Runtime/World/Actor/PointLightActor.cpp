#include "PointLightActor.h"
#include "World/LightManager.h"


namespace Zero
{
	UPointLightActor::UPointLightActor()
		:UMeshActor()
	{
		m_LightComponnet = CreateComponent<UPointLightComponnet>(this);
		m_MeshRenderComponent->AttachRenderLayer(RENDERLAYER_LIGHT, PSO_POINT_LIGHT);
		GetComponent<UMeshRenderComponent>()->SetEnableMaterial(false);
	}

	void UPointLightActor::PostInit()
	{
		Supper::PostInit();
		FLightManager::Get().AddPointLight(this);
	}
	

	void UPointLightActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshData MeshData;
		FMeshGenerator::Get().CreateSphere(MeshData, 0.5f , 1);
		MeshDatas.push_back(MeshData);
		m_MeshVertexComponent->CreateMesh();
	}
} 