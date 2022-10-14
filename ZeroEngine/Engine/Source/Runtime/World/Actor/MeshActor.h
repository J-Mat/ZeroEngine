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
		UMeshActor();
		virtual void BuildMesh() = 0;
		virtual void PostInit() override;
		virtual void CommitToPipieline();
		virtual void Tick();
		virtual ZMath::FAABB GetAABB() override;
	protected:
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		UMeshRenderComponent* m_MeshRenderComponent = nullptr;
	};
}