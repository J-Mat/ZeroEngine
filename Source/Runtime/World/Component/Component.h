#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"
#include "../Base/PropertyObject.h"

namespace Zero
{
	class UCoreObject;
	class UComponent : public UCoreObject
	{
	public:
		UComponent();
		void SetParentComponent(UComponent* Component);
		std::vector<UComponent*>& GetChidren() { return m_Chidren; }
		void AttachComponent(UComponent* Component) { m_Chidren.push_back(Component); };
	protected:
		std::vector<UComponent*> m_Chidren;
		UComponent* m_Parent = nullptr;
	};
}