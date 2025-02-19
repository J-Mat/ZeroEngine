#include "Component.h"
#include "../Object/CoreObject.h"

namespace Zero
{
	UComponent::UComponent()
		: UCoreObject()

	{

	}
	void UComponent::SetParentComponent(UComponent* Component)
	{
		m_Parent = Component; 
		if (m_Parent != nullptr)
		{
			m_Parent->AttachComponent(this);
		}
	}
}
