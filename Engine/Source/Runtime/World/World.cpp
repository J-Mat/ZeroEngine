#include "World.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/MeshActor.h"
#include "LightManager.h"

namespace Zero
{
	UWorld* UWorld::s_CurrentWorld = nullptr;
	UWorld::UWorld()
	{
		SetCurrentWorld(this);
		m_MainCamera = CreateActor<UCameraActor>();
		m_RenderItemPool.insert({ RENDERLAYER_OPAQUE, CreateRef<FRenderItemPool>()});
		m_RenderItemPool.insert({ RENDERLAYER_LIGHT, CreateRef<FRenderItemPool>()});
		m_RenderItemPool.insert({ RENDERLAYER_SKYBOX, CreateRef<FRenderItemPool>()});
		m_RenderItemPool.insert({ RENDERLAYER_SHADOW, CreateRef<FRenderItemPool>()});
		
		m_DIYRenderItemPool = CreateRef<FRenderItemPool>();
	}

	void UWorld::Tick()
	{
		FLightManager::Get().Tick();
		for (auto Iter : m_RenderItemPool)
		{
			Iter.second->Reset();
		}
		
		if (!m_bTick)
		{
			return;
		}
		
		for (UActor* Actor : m_Actors)
		{
			Actor->Tick();
		}
	}

	Ref<FRenderItemPool> UWorld::GetRenderItemPool(uint32_t RenderLayerType)
	{
		auto Iter = m_RenderItemPool.find(RenderLayerType);
		return Iter->second;
	}

	void UWorld::ClearAllActors()
	{
		m_Device->Flush();
		for (int i = 1; i < m_Actors.size(); ++i)
		{
			delete m_Actors[i];
		}
		m_Actors.clear();
		m_Actors.push_back(m_MainCamera);
	}

	void UWorld::PickActorByLayer(float& MinValue, uint32_t RenderLayer, ZMath::FRay ViewRay, UActor*& PickActor)
	{
		const auto& GuidList = m_RenderItemPool[RenderLayer]->GetRenderGuids();
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
					PickActor = Actor;
					MinValue = std::min(MinValue, IntersectValue);
				}
			}
		}
	}

	UActor* UWorld::PickActorByMouse(ZMath::FRay Ray)
	{
		float MinT = std::numeric_limits<float>::max();
		UActor* Result = nullptr;
		const auto& ViewMat = m_MainCamera->GetComponent<UCameraComponent>()->GetView();
		const auto& InvView = ZMath::inverse(ViewMat);
		ZMath::FRay ViewRay =  Ray.TransformRay(InvView);
		const auto& GuidList_1 = m_RenderItemPool[RENDERLAYER_OPAQUE]->GetRenderGuids();
		PickActorByLayer(MinT, RENDERLAYER_OPAQUE, ViewRay,  Result);
		PickActorByLayer(MinT, RENDERLAYER_LIGHT, ViewRay,  Result);
		return Result;
	}
	ZMath::vec3 UWorld::GetRayWorldPos(ZMath::FRay& Ray, float Distance)
	{
		const auto& ViewMat = m_MainCamera->GetComponent<UCameraComponent>()->GetView();
		const auto& InvView = ZMath::inverse(ViewMat);
		ZMath::FRay ViewRay =  Ray.TransformRay(InvView);
		return ViewRay.GetPos(Distance);
	}
}
