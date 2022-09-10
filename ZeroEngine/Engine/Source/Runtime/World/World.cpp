#include "World.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/MeshActor.h"

namespace Zero
{
	UWorld* UWorld::s_CurrentWorld = nullptr;
	UWorld::UWorld()
	{
		SetCurrentWorld(this);
		m_MainCamera = new UCameraActor();
	}
	void UWorld::Tick()
	{
		if (!m_bTick)
		{
			return;
		}
		
		m_MainCamera->Tick();
		
		m_RenderItemPool.Reset();
		for (UMeshActor* MeshActor : m_MeshActors)
		{
			MeshActor->Tick();
			MeshActor->CommitToPipieline(m_RenderItemPool);
		}
		
	}
	void UWorld::PushActor(UActor* Actor)
	{
		m_Actors.push_back(Actor);
	}
}
