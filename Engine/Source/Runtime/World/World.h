#pragma once

#include "Core.h"
#include "Render/RendererAPI.h"
#include "Object/CoreObject.h"
#include "Render/RHI/GraphicDevice.h"
#include "Actor/Actor.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/CameraActor.h"
#include "Actor/SkyActor.h"

namespace Zero
{
	class UMeshActor;
	class UWorld : public UCoreObject
	{
	public:
		static UWorld* CreateWorld(uint32_t DeviceSlot = 0)
		{
			auto* World = new UWorld();
			World->SetDevice(FGraphic::GetDevice(DeviceSlot));
			return World;
		}
		UWorld();
		Ref<IDevice> GetDevice() { return m_Device; }
		void SetDevice(Ref<IDevice> Device) { m_Device = Device; }
		Ref<FRenderItemPool> GetDIYRenderItemPool() { return m_DIYRenderItemPool; }
		virtual void Tick();
		Ref<FRenderItemPool> GetRenderItemPool(ERenderLayer RenderLayerType);
		static  UWorld* GetCurrentWorld() { return s_CurrentWorld; }
		static void  SetCurrentWorld(UWorld* World) { s_CurrentWorld = World; }
		void SetCamera(UCameraActor* Camera) { m_MainCamera = Camera; }
		UCameraActor* GetMainCamera() { return m_MainCamera; }
		USkyActor* GetSkyActor() { return m_SkyActor; }
		std::vector<UActor*> GetActors() { return m_Actors; }

		template<class T, typename ...ParamTypes>
		T* CreateActor(ParamTypes &&...Params)
		{
			T* Actor = new T(std::forward<ParamTypes>(Params)...);
			Actor->SetWorld(this);
			Actor->InitReflectionContent();
			Actor->PostInit();
			AddActor(Actor);
			return Actor;
		}

		template<>
		USkyActor* CreateActor()
		{
			USkyActor* Actor = new USkyActor();
			Actor->SetWorld(this);
			Actor->InitReflectionContent();
			Actor->PostInit();
			AddActor(Actor);
			m_SkyActor = Actor;
			return Actor;
		}

		template<class T>
		T* CreateActorRaw()
		{
			T* Actor = new T();
			Actor->SetWorld(this);
			Actor->InitReflectionContent();
			AddActor(Actor);
			return Actor;
		}
		
		void ClearAllActors();

		void AddActor(UActor* Actor)
		{
			m_Actors.push_back(Actor);
		}		
		
		UActor* PickActorByMouse(ZMath::FRay Ray);
		void PickActorByLayer(float& MinValue, ERenderLayer RenderLayer, ZMath::FRay ViewRay,  UActor* &PickActor);
		ZMath::vec3 GetRayWorldPos(ZMath::FRay& Ray, float Distance = 2.0f);
		
		void SetSceneName(std::string SceneName) { m_SceneName = SceneName; }
		const std::string& GetSceneName() { return m_SceneName; }

	private:
		static UWorld* s_CurrentWorld;
		UCameraActor* m_MainCamera = nullptr;
		USkyActor* m_SkyActor = nullptr;
		std::map<ERenderLayer, Ref<FRenderItemPool>> m_RenderItemPool;
		Ref<FRenderItemPool> m_DIYRenderItemPool;
		Ref<IDevice> m_Device;
		std::vector<UActor*> m_Actors;
		std::vector<UMeshActor*> m_MeshActors;
		std::string m_SceneName;
	};
}