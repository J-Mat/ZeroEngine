#pragma once
#include "MeshActor.h"
#include "World/Component/LightComponent/DirectLightComponent.h"
#include "World/Component/MeshRenderComponent.h"
#include "World/Component/MeshVertexComponent.h"

namespace Zero
{
	enum class ELightType
	{
		LT_Direct,
		LT_Point
	};
	class ULightActor : public UMeshActor
	{
	public:
		ULightActor(ELightType LightType);
		virtual void BuildMesh();
		virtual void PostInit() override;
		

	private:
		ULightComponent* m_LightComponnet = nullptr;
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		ELightType m_LightType = ELightType::LT_Direct;
	};
}