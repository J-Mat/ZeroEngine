#pragma once

#include "Core.h"
#include "GUIDInterface.h"
#include "World/Construction/ObjectConstruction.h"
#include "World/World.h"

namespace Zero
{
	class UCoreObject : public IGUIDInterface
	{
	public:
		UCoreObject();
		virtual ~UCoreObject();
		static std::vector<UCoreObject*> s_ObjectsCollection;
		bool IsTick()const { return m_bTick; }
		void SetWorld(UWorld* World) { m_World = World; }
		UWorld* GetWorld() { return m_World; }
	protected:
		bool m_bTick = true;
		uint32_t m_ = 0;
		UWorld* m_World;
	};
}