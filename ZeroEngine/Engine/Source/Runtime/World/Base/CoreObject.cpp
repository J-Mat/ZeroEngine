#include "CoreObject.h"


namespace Zero
{
	std::map<Utils::Guid, UCoreObject*>  UCoreObject::s_ObjectsCollection = {};
	UCoreObject::UCoreObject()
		: IGUIDInterface()
	{
		s_ObjectsCollection.insert({m_GUID, this});
	}


	UCoreObject::~UCoreObject()
	{
		s_ObjectsCollection.erase(m_GUID);
	}
	UCoreObject* UCoreObject::GetObjByGuid(const Utils::Guid& Guid)
	{
		auto Iter = s_ObjectsCollection.find(Guid);
		if (Iter != s_ObjectsCollection.end())
		{
			return Iter->second;
		}
		return nullptr;
	}

	void UCoreObject::SetOuter(UCoreObject* InNewOuter)
	{
		m_Outer = InNewOuter; 
	}

	void UCoreObject::InitCoreObject()
	{
		m_ClassInfoCollection.m_Outer = this;
		InitReflectionContent();
	}

}
