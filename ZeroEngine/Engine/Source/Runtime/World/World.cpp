#include "World.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/MeshActor.h"

namespace Zero
{
	UWorld* UWorld::s_CurrentWorld = nullptr;
	UWorld::UWorld()
	{
		SetCurrentWorld(this);
		m_MainCamera = CreateActor<UCameraActor>();
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
	UActor* UWorld::PickActorByMouse(ZMath::FRay Ray)
	{
		float MinT = std::numeric_limits<float>::max();
		UActor* Result = nullptr;
		const auto& ViewMat = m_MainCamera->GetComponent<UCameraComponent>()->GetView();
		const auto& InvView = ZMath::inverse(ViewMat);
		ZMath::FRay ViewRay =  Ray.TransformRay(InvView);
		const auto& GuidList = m_RenderItemPool.GetRenderGuids();
		for (const auto& Guid : GuidList)
		{
			UCoreObject* Obj = GetObjByGuid(Guid);
			if (Obj != nullptr)
			{
				auto* Actor = static_cast<UActor*>(Obj);
				auto* TransformComponent = Actor->GetComponent<UTransformComponent>();
				const ZMath::mat4 Transform = ZMath::inverse(TransformComponent->GetTransform());
				
				ZMath::FRay TransformRay = ViewRay.TransformRay(Transform);
				float IntersectValue;
				if (TransformRay.IntersectsAABB(Actor->GetAABB(), IntersectValue))
				{
					Result = Actor;
					MinT = std::min(MinT, IntersectValue);
				}
			}
		}
		return Result;
	}
}
