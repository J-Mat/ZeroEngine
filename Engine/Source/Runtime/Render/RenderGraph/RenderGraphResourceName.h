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
		constexpr FRGResourceName DirectLigthShadowMap_0 =  RG_RES_NAME(DirectLigthShadowMap_0);
		constexpr FRGResourceName DirectLightShadowMap_1 =  RG_RES_NAME(DirectLightShadowMap_1);
		constexpr FRGResourceName DirectLightShadowMaps[2] = {DirectLigthShadowMap_0 , DirectLightShadowMap_1};

		constexpr FRGResourceName PointLigthShadowMap_0 =  RG_RES_NAME(PointLigthShadowMap_0);
		constexpr FRGResourceName PointLightShadowMap_1 =  RG_RES_NAME(PointLightShadowMap_1);
		constexpr FRGResourceName PointLigthShadowMap_2 =  RG_RES_NAME(PointLigthShadowMap_2);
		constexpr FRGResourceName PointLightShadowMap_3 =  RG_RES_NAME(PointLightShadowMap_3);
		constexpr FRGResourceName PointLightShadowMaps[4] = { PointLigthShadowMap_0 , PointLightShadowMap_1, PointLigthShadowMap_2 ,PointLightShadowMap_3  };

		constexpr FRGResourceName PointLigthShadowMapDebug_0 =  RG_RES_NAME(PointLigthShadowMapDebug_0);
		constexpr FRGResourceName PointLightShadowMapDebugs[1] = { PointLigthShadowMapDebug_0};

		constexpr FRGResourceName ShadowMap_Debug_0 =  RG_RES_NAME(ShadowMap_Debug_0);
		constexpr FRGResourceName ShadowMap_Debug_1 =  RG_RES_NAME(ShadowMap_Debug_1);
		constexpr FRGResourceName ShadowMaps_Debug[2] = {ShadowMap_Debug_0 , ShadowMap_Debug_1};

		constexpr FRGResourceName BufferArray = RG_RES_NAME(BufferArray);
		constexpr FRGResourceName ResultBuffer = RG_RES_NAME(ResultBuffer);
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


