#include "CoreObject.h"
#include "ObjectGlobal.h"

namespace Zero
{
	UCoreObject::UCoreObject()
		: IGUIDInterface()
	{
		FObjectGlobal::GetObjectCollection().insert({m_GUID, this});
		m_ClassInfoCollection.m_Outer = this;
	}


	UCoreObject::~UCoreObject()
	{
		FObjectGlobal::GetObjectCollection().erase(m_GUID);
	}
	UCoreObject* UCoreObject::GetObjByGuid(const Utils::Guid& Guid)
	{
		auto Iter = FObjectGlobal::GetObjectCollection().find(Guid);
		if (Iter != FObjectGlobal::GetObjectCollection().end())
		{
			return Iter->second;
		}
		return nullptr;
	}

	void UCoreObject::SetOuter(UCoreObject* InNewOuter)
	{
		m_Outer = InNewOuter; 
	}
}
