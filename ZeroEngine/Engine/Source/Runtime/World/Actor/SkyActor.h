#pragma once

#include "Core.h"
#include "Actor.h"
#include "Render/Moudule/MeshGenerator.h"
#include "SkyActor.reflection.h"

namespace Zero
{
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class FTextureCubemap;
	class FRenderItemPool;
	UCLASS()
	class USkyActor : public UActor
	{
		GENERATED_BODY()
	public:
		USkyActor();
		virtual void BuildMesh();
		virtual void PostInit() override;
		virtual void CommitToPipieline();
		virtual void Tick();
		void SetTextureCubemap(const std::string& TextureName, Ref<FTextureCubemap> TextureCubeMap);
	protected:
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		UMeshRenderComponent* m_MeshRenderComponent = nullptr;
	};
}