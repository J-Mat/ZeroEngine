#pragma once
#include "MeshActor.h"
#include "World/Component/LightComponent/PointLightComponent.h"
#include "World/Component/MeshRenderComponent.h"
#include "World/Component/MeshVertexComponent.h"
#include "PointLightActor.reflection.h"

namespace Zero
{
	class FSceneCaptureCube;
	UCLASS()
	class UPointLightActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		UPointLightActor();
		void PostInit();
		virtual void BuildMesh() override;

		const FSceneCaptureCube& GetSceneCaptureCube() { return m_LightComponnet->GetSceneCaptureCube(); }
	private:
		UPointLightComponnet* m_LightComponnet = nullptr;
	};
}