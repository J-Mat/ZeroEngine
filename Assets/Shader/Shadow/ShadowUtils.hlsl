#ifndef __SHADER_SHADOW__
#define __SHADER_SHADOW__

#include "../Sampler.hlsl"
#include "../Utils.hlsl"

#define BLOCKER_SEARCH_SAMPLE_COUNT    20
#define BLOCKER_SEARCH_PIXEL_RADIUS    10.0f 

#define PCF_SAMPLE_COUNT           30
#define PCF_SAMPLE_PIXLE_RADIUS    3

float BlockerSearch(float3 ReceiverPos, float dx, uint ShadowMapIndex)
{
	float AverageBlockerDepth = 0.0f;
    int BlockerCount = 0;

	const int SampleCount = BLOCKER_SEARCH_SAMPLE_COUNT;
	const float SearchWidth = BLOCKER_SEARCH_PIXEL_RADIUS * dx;
	
	for (int i = 0; i < SampleCount; ++i)
	{
		float2 UVOffset = (Hammersley(i, SampleCount) * 2.0f - 1.0f) * SearchWidth;
		
		float2 SampleUV = ReceiverPos.xy + UVOffset;
		float BloackDepth = _gShadowMaps[ShadowMapIndex].Sample(gSamLinearClamp, SampleUV).r;

		if (BloackDepth < ReceiverPos.z)
		{
			AverageBlockerDepth += BloackDepth;
			BlockerCount++;
		}
	}
	
	return BlockerCount > 0 ? AverageBlockerDepth / BlockerCount : -1.0f;
}

float PCF(float3 ReceiverPos, float UVRadius, uint ShadowMapIndex)
{
	const int SampleCount = PCF_SAMPLE_COUNT;
	float Visibility = 0.0f;
	for (int i = 0; i < SampleCount; ++i)
	{
		float2 UVOffset = (Hammersley(i, SampleCount) * 2.0f - 1.0f) * UVRadius;
		float2 SampleUV = ReceiverPos.xy + UVOffset;
		
		Visibility += _gShadowMaps[ShadowMapIndex].SampleCmpLevelZero(gSamShadow, SampleUV, ReceiverPos.z).r;
	}
	
	return Visibility / SampleCount;
}

float PCSS(float3 ReceiverPos, float dx, uint ShadowMapIndex, float LightPixelWidth)
{
	float ReceiverDepth = ReceiverPos.z;
	
	float BlockDepth = BlockerSearch(ReceiverPos, dx, ShadowMapIndex);

	if (BlockDepth < 0.0f)
	{
		return 1.0f;
	}
	
	if (BlockDepth > 1.0f)
	{
		return 0.0f;
	}

	const float LightWidth = LightPixelWidth;

	float PenumbraWidth = (ReceiverDepth - BlockDepth) * LightWidth / BlockDepth;

	return PCF(ReceiverPos, PenumbraWidth * dx,  ShadowMapIndex);
}

float CalcShadowFactor(int ShadowMapIndex, float4 ShadowPos, float LightPixelWidth)
{
	float4 ProjCoords = ShadowPos;
	ProjCoords.xyz /= ProjCoords.w;

    float2 ShadowTexCoord = 0.5f * ProjCoords.xy + 0.5f;
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

	float3 ReceiverPos = float3(ShadowTexCoord.xy, ProjCoords.z);
    uint Width, Height, NumMips;
    _gShadowMaps[ShadowMapIndex].GetDimensions(0, Width, Height, NumMips);

    // Texel size.
    float dx = 1.0f / Width;

	
	return PCSS(ReceiverPos, dx,  ShadowMapIndex, LightPixelWidth);
#if USE_PCSS
	return PCSS(ReceiverPos, dx,  ShadowMapIndex, LightPixelWidth);
#else // USE_PCF
	return PCF(ReceiverPos, PCF_SAMPLE_PIXLE_RADIUS * dx, ShadowMapIndex);
#endif
}

float CalcVisibilityOmni(float3 LightToPoint, uint3 ShadowMapIndex, float FarZ)
{
	float ClosestDepth = _gShadowMapCubes.SampleLevel(gSamPointClamp, normalize(LightToPoint), 0).r;

	ClosestDepth *= FarZ;
	
	float CurrentDepth = length(LightToPoint);

	return CurrentDepth > ClosestDepth? 1.0f : 0.0f;
	return 1.0f;
}

#endif