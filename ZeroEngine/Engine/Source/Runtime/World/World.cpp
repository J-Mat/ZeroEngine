#include "World.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/MeshActor.h"

namespace Zero
{
	UWorld* UWorld::s_CurrentWorld = nullptr;
	UWorld::UWorld()
	{
		SetCurrentWorld(this);
		m_MainCamera = UActor::Create<UCameraActor>(this);
		AddActor(m_MainCamera);
	}
	void UWorld::Tick()
	{
		m_RenderItemPool.Reset();
		
		if (!m_bTick)
		{
			return;
		}
		
		for (UActor* Actor : m_Actors)
		{
			Actor->Tick();
		}
	}
}
