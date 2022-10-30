#pragma once

#include "Core.h"
#include "GUIDInterface.h"
#include "ObjectMacros.h"
#include "ClassInfoCollection.h"

namespace Zero
{
	struct FRawInitFlag 
	{
		FRawInitFlag() = default;
	};
	class UCoreObject : public IGUIDInterface
	{
	public:
		UCoreObject(FRawInitFlag Flag) {};
		UCoreObject();
		virtual ~UCoreObject();
		static UCoreObject* GetObjByGuid(const Utils::Guid& Guid);
		bool IsTick()const { return m_bTick; }
		virtual inline void SetOuter(UCoreObject* InNewOuter);
		virtual void PostInit() {}
		virtual void Tick() {}
		UCoreObject* GetOuter() { return m_Outer; }
		virtual void InitReflectionContent() {}
		virtual const char* GetObjectName() { return "UCoreObject"; } 
		
#ifdef EDITOR_MODE 
		virtual void PostEdit(UProperty* Property) {};
#endif
	public:
		FClassInfoCollection m_ClassInfoCollection;
		FClassInfoCollection& GetClassCollection() { return m_ClassInfoCollection; }

	protected:
		bool m_bTick = true;
		UCoreObject* m_Outer;
	};
}