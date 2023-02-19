#pragma once

#include "Core.h"
#include "MeshActor.h"
#include "Render/Moudule/MeshGenerator.h"
#include "SkyActor.reflection.h"

namespace Zero
{
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class FTextureCubemap;
	class FRenderItemPool;
	UCLASS()
	class USkyActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		USkyActor();
		virtual void BuildMesh();
		void SetTextureCubemap(const std::string& TextureName, Ref<FTextureCubemap> TextureCubeMap);
	};
}