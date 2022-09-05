#pragma once

#include "Core.h"
#include "Actor.h"

namespace Zero
{
	class UMeshVertexComponent;
	class UMeshRenderComponent;
	class FRenderItemPool;
	class UMeshActor : public UActor
	{
	public:
		UMeshActor(const std::string& Tag);
		virtual void PostInit();
		virtual void CommitToPipieline(FRenderItemPool& RenderItemPool);
	protected:
		UMeshVertexComponent* m_MeshVertexComponent = nullptr;
		UMeshRenderComponent* m_MeshRenderComponent = nullptr;
	};
}