#pragma once
#include "MeshActor.h"
#include "World/Component/LightComponent/DirectLightComponent.h"
#include "World/Component/MeshRenderComponent.h"
#include "World/Component/MeshVertexComponent.h"
#include "LightActor.reflection.h"

namespace Zero
{
	enum class ELightType
	{
		LT_Direct,
		LT_Point
	};
	UCLASS()
	class ULightActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		void PostInit();
		ULightActor();
		virtual void BuildMesh() override;
		

	private:
		ULightComponent* m_LightComponnet = nullptr;
		ELightType m_LightType = ELightType::LT_Direct;
	};
}