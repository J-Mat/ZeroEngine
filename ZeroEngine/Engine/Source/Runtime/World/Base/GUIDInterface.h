#pragma once

#include "Core.h"
#include "Core/Base/GUID.h"

namespace Zero
{
	class IGUIDInterface
	{
	public:
		IGUIDInterface();
		std::string ToString() const { return m_GUID.ToString(); }
		bool operator==(IGUIDInterface& Other)
		{
			return ToString() == Other.ToString();
		}
	private:
		FGUID m_GUID;
	};
}