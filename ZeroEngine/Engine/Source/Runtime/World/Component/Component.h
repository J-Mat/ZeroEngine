#pragma once

#include "Core.h"
#include "../Base/CoreObject.h"

namespace Zero
{
	class UCoreObject;
	class UComponent : public UCoreObject
	{
	public:
		UComponent();
		void SetParentComponent(UComponent* Component);
	protected:
		UComponent* m_Parent = nullptr;
	};
}