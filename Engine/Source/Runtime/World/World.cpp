#include "World.h"
#include "Actor/MeshActor.h"
#include "LightManager.h"

namespace Zero
{
	UWorld* UWorld::s_CurrentWorld = nullptr;
	UWorld::UWorld()
	{
		SetCurrentWorld(this);
		m_MainCamera = CreateActor<UCameraActor>();
		m_RenderItemPool.insert({ ERenderLayer::Opaque, CreateRef<FRenderItemPool>()});
		m_RenderItemPool.insert({ ERenderLayer::Light, CreateRef<FRenderItemPool>()});
		m_RenderItemPool.insert({ ERenderLayer::Skybox, CreateRef<FRenderItemPool>()});
		m_RenderItemPool.insert({ ERenderLayer::Shadow, CreateRef<FRenderItemPool>()});
		
		m_ComputeRenderItemPool = CreateRef<FComputeRenderItemPool>();
		m_DIYRenderItemPool = CreateRef<FRenderItemPool>();
	}

	void UWorld::Tick()
	{
		FLightManager::Get().Tick();
		for (auto Iter : m_RenderItemPool)
		{
			Iter.second->Reset();
		}
		m_ComputeRenderItemPool->Reset();
		
		if (!m_bTick)
		{
			return;
		}
		
		for (UActor* Actor : m_Actors)
		{
			Actor->Tick();
		}
	}

	Ref<FRenderItemPool> UWorld::GetRenderItemPool(ERenderLayer RenderLayerType)
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

	void UWorld::PickActorByLayer(float& MinValue, ERenderLayer RenderLayer, ZMath::FRay ViewRay, UActor*& PickActor)
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
		const FSceneView& SceneView = m_MainCamera->GetComponent<UCameraComponent>()->GetSceneView();
		const auto& ViewMat = SceneView.View;
		const auto& InvView = ZMath::inverse(ViewMat);
		ZMath::FRay ViewRay =  Ray.TransformRay(InvView);
		const auto& GuidList_1 = m_RenderItemPool[ERenderLayer::Opaque]->GetRenderGuids();
		PickActorByLayer(MinT, ERenderLayer::Opaque, ViewRay,  Result);
		PickActorByLayer(MinT, ERenderLayer::Light, ViewRay,  Result);
		return Result;
	}
	ZMath::vec3 UWorld::GetRayWorldPos(ZMath::FRay& Ray, float Distance)
	{
		const FSceneView& SceneView = m_MainCamera->GetComponent<UCameraComponent>()->GetSceneView();
		const auto& ViewMat = SceneView.View;
		const auto& InvView = ZMath::inverse(ViewMat);
		ZMath::FRay ViewRay =  Ray.TransformRay(InvView);
		return ViewRay.GetPos(Distance);
	}
}
