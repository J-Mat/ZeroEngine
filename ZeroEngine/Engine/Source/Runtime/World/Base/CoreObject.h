#pragma once

#include "Core.h"
#include "GUIDInterface.h"

namespace Zero
{
	class UWorld;
	class UCoreObject : public IGUIDInterface
	{
	public:
		UCoreObject();
		virtual ~UCoreObject();
		static std::vector<UCoreObject*> s_ObjectsCollection;
		bool IsTick()const { return m_bTick; }
		inline void SetWorld(UWorld* World) { m_World = World; }
		inline virtual UWorld* GetWorld();
		virtual void PostInit() {}
		virtual void Tick() {}
	protected:
		bool m_bTick = true;
		UWorld* m_World;
	};
}