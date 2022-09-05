#include "World.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/MeshActor.h"

namespace Zero
{
	UWorld* UWorld::s_CurrentWorld = nullptr;
	UWorld::UWorld()
	{
		SetCurrentWorld(this);
	}
	void UWorld::OnUpdate()
	{
		if (!m_bTick)
		{
			return;
		}
		
		m_RenderItemPool.Reset();
		for (UMeshActor* MeshActor : m_MeshActors)
		{
			MeshActor->OnUpdate();
			MeshActor->CommitToPipieline(m_RenderItemPool);
		}
	}
}
