#pragma once

#include "Core.h"
#include "GUIDInterface.h"

namespace Zero
{
	class UCoreObject : public IGUIDInterface
	{
	public: 
		UCoreObject();
		virtual ~UCoreObject();
		static std::vector<UCoreObject*> s_GObjects;
		bool IsTick()const { return m_bTick; }
	protected:
		bool m_bTick = true;
	};
}