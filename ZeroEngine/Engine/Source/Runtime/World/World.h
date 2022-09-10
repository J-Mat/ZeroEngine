#pragma once

#include "Core.h"
#include "Base/CoreObject.h"
#include "Render/RHI/GraphicDevice.h"
#include "Actor/Actor.h"
#include "Render/RHI/RenderItem.h"
#include "Actor/CameraActor.h"

namespace Zero
{
	class UMeshActor;
	class UWorld : public UCoreObject
	{
	public:
		UWorld();
		Ref<IDevice> GetDevice() { return m_Device; }
		void SetDevice(Ref<IDevice> Device) { m_Device = Device; }
		virtual void Tick();
		FRenderItemPool& GetRenderItemPool() { return m_RenderItemPool; }
		static  UWorld* GetCurrentWorld() { return s_CurrentWorld; }
		static void  SetCurrentWorld(UWorld* World) { s_CurrentWorld = World; }
		void SetCamera(UCameraActor* Camera) { m_MainCamera = Camera; }
		void PushActor(UActor* Actor);
	private:
		static UWorld* s_CurrentWorld;
		UCameraActor* m_MainCamera;
		FRenderItemPool m_RenderItemPool;
		Ref<IDevice> m_Device;
		std::vector<UActor*> m_Actors;
		std::vector<UMeshActor*> m_MeshActors;
	};
}