#include "CoreObject.h"
#include "World/World.h"


namespace Zero
{
	std::vector<UCoreObject*> UCoreObject::s_ObjectsCollection = {};
	UCoreObject::UCoreObject()
	{
		s_ObjectsCollection.push_back(this);
	}


	UCoreObject::~UCoreObject()
	{
		for (std::vector<UCoreObject*>::const_iterator Iter = s_ObjectsCollection.begin(); Iter != s_ObjectsCollection.end(); ++Iter)
		{
			if (*Iter == this)
			{
				s_ObjectsCollection.erase(Iter);
				break;
			}
		}
	}
	void UCoreObject::SetWorld(UWorld* World)
	{
		m_World = World;
	}
	UWorld* UCoreObject::GetWorld()
	{
		return m_World;
	}
}
