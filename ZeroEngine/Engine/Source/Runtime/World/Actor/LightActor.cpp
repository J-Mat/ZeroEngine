#include "LightActor.h"


namespace Zero
{
	ULightActor::ULightActor(ELightType LightType)
		: UMeshActor(),
		m_LightType(LightType)
	{
	}

	void ULightActor::PostInit()
	{
		switch (m_LightType)
		{
		case Zero::ELightType::LT_Direct:
			m_LightComponnet = CreateComponent<UDirectLightComponnet>(this);
			break;
		case Zero::ELightType::LT_Point:
			break;
		default:
			break;
		}
	}
	

	void ULightActor::BuildMesh()
	{
		auto& MeshDatas =  m_MeshVertexComponent->GetMeshDatas();
		FMeshGenerator::GetInstance().CreateCustomModel(MeshDatas, ZConfig::GetObjFullPath("Sphere.fbx").string(), FVertexBufferLayout::s_DefaultVertexLayout);
		
		m_MeshVertexComponent->CreateMesh();
	}
} 