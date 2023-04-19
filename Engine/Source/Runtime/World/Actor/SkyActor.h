#pragma once

#include "Core.h"
#include "MeshActor.h"
#include "Render/Moudule/MeshGenerator.h"
#include "SkyActor.reflection.h"

namespace Zero
{
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class FTextureCube;
	class FRenderItemPool;
	UCLASS()
	class USkyActor : public UMeshActor
	{
		GENERATED_BODY()
	public:
		USkyActor();
		virtual void BuildMesh();
		virtual void SetCustomParemeters() override;
		void SetTextureCubemap(const std::string& TextureName, Ref<FTextureCube> TextureCubeMap);
	private:
		std::string m_TextureCubeMapName;
		Ref<FTextureCube> m_TextureCubeMap;
	};
}