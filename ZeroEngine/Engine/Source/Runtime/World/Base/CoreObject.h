#pragma once

#include "Core.h"
#include "GUIDInterface.h"
#include "ObjectMacros.h"
#include "ClassInfoCollection.h"
#include "CoreObject.reflection.h"

namespace Zero
{
	static std::set<std::string> g_AllUObjects;
	UCLASS()
	class UCoreObject : public IGUIDInterface
	{
		GENERATED_BODY()
	private:
		static std::map<std::string, uint32_t> s_ObjectNameIndex;
	public:
		UCoreObject();
		virtual ~UCoreObject();
		static std::map<Utils::Guid, UCoreObject*> s_ObjectsCollection;
		static UCoreObject* GetObjByGuid(const Utils::Guid& Guid);
		bool IsTick()const { return m_bTick; }
		virtual inline void SetOuter(UCoreObject* InNewOuter);
		virtual void PostInit() {}
		virtual void Tick() {}
		
		void SetName(const std::string& Name) 
		{ 
			auto Iter = s_ObjectNameIndex.find(Name);
			if (Iter == s_ObjectNameIndex.end())
			{
				s_ObjectNameIndex.insert({Name, 1});
				m_Name = Name;
			}
			else
			{
				uint32_t Index = Iter->second++;
				m_Name = std::format("{0}_{1}",Name, Index);
			}
		}
		const std::string& GetName() 
		{
			if (m_Name == "")
			{
				SetName(GetObjectName());
			}
			return m_Name;
		}
		UCoreObject* GetOuter() { return m_Outer; }
		

#ifdef EDITOR_MODE 
		virtual void PostEdit(UProperty* Property) {};
#endif
		
	public:
		FClassInfoCollection m_ClassInfoCollection;
		FClassInfoCollection& GetClassCollection() { return m_ClassInfoCollection; }

	protected:
		bool m_bTick = true;
		UCoreObject* m_Outer;
	public:
		std::string m_Name = "";
	};
}