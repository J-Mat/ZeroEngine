#pragma once
#include "MeshActor.h"
#include "World/Component/LightComponent/DirectLightComponent.h"
#include "World/Component/MeshRenderComponent.h"
#include "World/Component/MeshVertexComponent.h"
#include "DirectLightActor.reflection.h"

namespace Zero
{
	UCLASS()
	class UDirectLightActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		void PostInit();
		UDirectLightActor();
		virtual void BuildMesh() override;
		

	private:
		ULightComponent* m_LightComponnet = nullptr;
	};
}