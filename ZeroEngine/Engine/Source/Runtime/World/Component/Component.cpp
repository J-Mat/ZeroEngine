#include "Component.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	UComponent::UComponent()
		: UCoreObject()

	{

	}
	void UComponent::SetParentComponent(UComponent* Component)
	{
		m_Parent = Component; 
		m_Parent->AttachComponent(this);
	}
}
