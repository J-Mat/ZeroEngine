#pragma once

#include "Core.h"
#include "GUIDInterface.h"
#include "ObjectMacros.h"

namespace Zero
{
	class FClassInfoCollection;
	class UCoreObject : public IGUIDInterface
	{
	public:
		UCoreObject();
		virtual ~UCoreObject();
		static std::map<Utils::Guid, UCoreObject*> s_ObjectsCollection;
		static UCoreObject* GetObjByGuid(const Utils::Guid& Guid);
		bool IsTick()const { return m_bTick; }
		virtual inline void SetOuter(UCoreObject* InNewOuter) { m_Outer = InNewOuter; }
		virtual void PostInit() {}
		virtual void Tick() {}
		
		void SetName(std::string Name) { m_Name = Name; }
		const std::string& GetName() const { return m_Name; }

	public:
		virtual void InitCoreObject();
		virtual void InitReflectionContent() {};
	protected:
		FClassInfoCollection m_ClassInfoCollection;

	protected:
		bool m_bTick = true;
		UCoreObject* m_Outer;
		std::string m_Name;
		

	};
}