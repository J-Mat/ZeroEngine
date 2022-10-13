#pragma once

#include "Core.h"
#include "GUIDInterface.h"
#include "ObjectMacros.h"

namespace Zero
{
	class UWorld;
	class UCoreObject : public IGUIDInterface
	{
	public:
		UCoreObject();
		virtual ~UCoreObject();
		static std::map<Utils::Guid, UCoreObject*> s_ObjectsCollection;
		static UCoreObject* GetObjByGuid(const Utils::Guid& Guid);
		bool IsTick()const { return m_bTick; }
		inline void SetWorld(UWorld* World) { m_World = World; }
		inline void SetOuter(UCoreObject* InNewOuter) { m_Outer = InNewOuter; }
		inline virtual UWorld* GetWorld();
		virtual void PostInit() {}
		virtual void Tick() {}
		void SetName(std::string Name) { m_Name = Name; }
		const std::string& GetName() const { return m_Name; }
	protected:
		bool m_bTick = true;
		UWorld* m_World;
		UCoreObject* m_Outer;
		std::string m_Name;
	};
}