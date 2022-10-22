#pragma once

#include "Core.h"

namespace Zero
{
	class IGUIDInterface
	{
	public:
		IGUIDInterface();
		Utils::Guid GetGuid() { return m_GUID; };
		std::string GetGuidString() { return m_GUID.str(); }
		
	protected:
		Utils::Guid m_GUID;
	};
}