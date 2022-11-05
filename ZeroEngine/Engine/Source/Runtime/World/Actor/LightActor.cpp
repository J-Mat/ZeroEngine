#include "LightActor.h"
#include "World/LightManager.h"


namespace Zero
{
	ULightActor::ULightActor()
		:UMeshActor()
		,m_LightType(ELightType::LT_Direct)
	{
		switch (m_LightType)
		{
		case ELightType::LT_Direct:
			m_LightComponnet = CreateComponent<UDirectLightComponnet>(this);
			break;
		case ELightType::LT_Point:
			break;
		default:
			break;
		}
		GetComponent<UMeshRenderComponent>()->SetShader("DirectLight.hlsl");
	}

	void ULightActor::PostInit()
	{
		Supper::PostInit();
		FLightManager::GetInstance().AddDirectLight(this);
	}
	

	void ULightActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::GetInstance().CreateCustomModel(MeshDatas, ZConfig::GetObjFullPath("direct_light.fbx").string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
} 