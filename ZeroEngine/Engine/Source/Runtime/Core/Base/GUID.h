#pragma once

#include "Core.h"

namespace Zero
{
	#define GUID_LEN 64 
	class FGUID
	{
	private:
		static uint32_t HashIndex;
	public:
		FGUID(GUID Guid);
		static FGUID GenerateGUID();
		std::string ToString() const; 
		uint64_t GetHashValue() const { return m_HashValue; }
	private:
		GUID m_GUID;
		uint64_t m_HashValue  = 0;
	};
}

namespace std
{
	template<>
	struct hash<Zero::FGUID>
	{
		std::size_t operator()(const Zero::FGUID& uuid) const
		{
			return hash<uint64_t>()(uuid.GetHashValue());
		}
	};
}