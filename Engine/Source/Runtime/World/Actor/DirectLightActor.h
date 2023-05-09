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

		FSceneCapture2D& GetSceneCapture2D() { return m_LightComponnet->GetSceneView(); }
	private:
		UDirectLightComponnet* m_LightComponnet = nullptr;
	};
}