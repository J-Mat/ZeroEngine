#pragma once

#include "Core.h"
#include "Actor.h"
#include "Render/Moudule/MeshGenerator.h"

namespace Zero
{
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class FRenderItemPool;
	class UMeshActor : public UActor
	{
	public:
		UMeshActor(const std::string& Tag = "Mesh", FMeshType& MeshType = FMeshType::s_CubeMeshType);
		virtual void PostInit();
		virtual void BuildMesh() = 0;
		virtual void CommitToPipieline();
		virtual void Tick();
	protected:
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		UMeshRenderComponent* m_MeshRenderComponent = nullptr;
		FMeshType m_MeshType;
	};
}