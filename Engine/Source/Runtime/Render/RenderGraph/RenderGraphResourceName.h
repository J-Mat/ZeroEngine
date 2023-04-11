#pragma once
#include "Utils/HashUtil.h"

#define RG_RES_NAME(name) FRGResourceName(#name, Zero::crc64(#name)) 
#define RG_RES_NAME_IDX(name, idx) FRGResourceName(#name, Zero::crc64(#name) + idx)
#define RG_STR_RES_NAME(name) FRGResourceName(name) FRGResourceName(name, Zero::crc64(name)) 
#define RG_STR_RES_NAME_IDX(name, idx) FRGResourceName(name, idx) FRGResourceName(name, Zero::crc64(name) + idx)



namespace Zero
{
	struct FRGResourceName
	{
		static constexpr uint64_t InvalidHash = uint64_t(-1);

		template<size_t N>
		constexpr explicit FRGResourceName(char const (&_name)[N], uint64_t hash) : HashedName(hash), Name(_name)
		{}

		operator char const* () const
		{
			return Name;
		}

		uint64_t HashedName;
		char const* Name;

		
	};

	inline bool operator==(FRGResourceName const& name1, FRGResourceName const& name2)
	{
		return name1.HashedName == name2.HashedName;
	}

	namespace RGResourceName
	{
		constexpr FRGResourceName FinalTexture = RG_RES_NAME(FinalTexture);
		constexpr FRGResourceName DepthStencil = RG_RES_NAME(DepthStencil);
		constexpr FRGResourceName GBufferColor = RG_RES_NAME(GBufferColor);
	}
}


namespace std
{
	template <> struct hash<Zero::FRGResourceName>
	{
		size_t operator()(Zero::FRGResourceName const& res_name) const
		{
			return hash<decltype(res_name.HashedName)>()(res_name.HashedName);
		}
	};
}


