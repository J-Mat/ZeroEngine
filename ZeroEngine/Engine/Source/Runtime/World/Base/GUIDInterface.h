#pragma once

#include "Core.h"

namespace Zero
{
	class IGUIDInterface
	{
	public:
		IGUIDInterface();
	protected:
		Utils::Guid m_GUID;
	};
}