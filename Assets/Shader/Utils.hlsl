

#ifndef UTILS_HLSL
#define UTILS_HLSL

#include "./Sampler.hlsl"


static const float F0_DIELECTRIC = 0.04f;
static const float PI = 3.14159265359f;

float Square( float x )
{
	return x*x;
}

float Pow4( float x )
{
	float xx = x*x;
	return xx * xx;
}


float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i, uint N)
{
	return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float Roughness)
{
	float a = Roughness * Roughness;
	float a2 = a * a;

	float Phi = 2.0 * PI * Xi.x;
	float CosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a2 - 1.0) * Xi.y));
	float SinTheta = sqrt(1.0 - CosTheta * CosTheta);

	// From spherical coordinates to cartesian coordinates
	float3 H;
	H.x = cos(Phi) * SinTheta;
	H.y = sin(Phi) * SinTheta;
	H.z = CosTheta;

	float3 Up = abs(N.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 TangentX = normalize(cross(Up, N));
	float3 TangentY = cross(N, TangentX);

	return TangentX * H.x + TangentY * H.y + N * H.z;
}

#define IBL_PREFILTER_ENVMAP_MIP_LEVEL 5

float3 GetPrefilteredColor(TextureCube IBLPrefilterMap, float Roughness, float3 ReflectDir)
{
    float Level = Roughness * (IBL_PREFILTER_ENVMAP_MIP_LEVEL - 1);
    
    float3 PrefilteredColor = IBLPrefilterMap.SampleLevel(gSamLinearClamp, ReflectDir, Level).rgb;

    return PrefilteredColor;

}

float3 EnvBRDF(float Metallic, float3 BaseColor, float2 LUT)
{
    float3 F0 = lerp(F0_DIELECTRIC.rrr, BaseColor.rgb, Metallic); 
    
    return F0 * LUT.x + LUT.y;
}

float3 AmbientLighting(float Metallic, float3 BaseColor, float3 Irradiance, float3 PrefilteredColor, float2 LUT)
{
    // IBL diffuse   
    float3 DiffuseColor = (1.0f - Metallic) * BaseColor; // Metallic surfaces have no diffuse reflections
    float3 DiffuseContribution = DiffuseColor  * Irradiance;
    
    // IBL specular
    float3 SpecularContribution = PrefilteredColor * EnvBRDF(Metallic, BaseColor, LUT);

    float3 Ambient = ( DiffuseContribution + SpecularContribution);
    return Ambient;
}

float CalcDistanceAttenuation(float3 WorldPos, float3 LightPos, float Range)
{
    float3 WorldLightVector = LightPos - WorldPos;
	float DistanceSqr = dot(WorldLightVector, WorldLightVector);
    
    float DistanceAttenuation = 1 / ( DistanceSqr + 1 );
    
    float InvRange = rcp(max(Range, 0.001f));
    float LightRadiusMask = Square(saturate(1 - Square(DistanceSqr * InvRange * InvRange)));
	DistanceAttenuation *= LightRadiusMask;
    
    return DistanceAttenuation;
}


float3 LambertDiffuse(float3 DiffuseColor)
{   
    return DiffuseColor * (1 / PI); 
}

float GGX(float a2, float NoH)
{
    float NoH2 = NoH * NoH;
    float d = NoH2 * (a2 - 1.0f) + 1.0f;

    return a2 / (PI * d * d);
}

float3 FresnelSchlick(float3 F0, float VoH)
{
    return F0 + (1 - F0) * exp2((-5.55473 * VoH - 6.98316) * VoH);
}

float GeometrySchlickGGX(float Roughness, float NoV)
{
    float k = pow(Roughness + 1, 2) / 8.0f;

    return NoV / (NoV * (1 - k) + k);
}

float3 SpecularGGX(float3 N, float3 L, float3 V, float Roughness, float3 F0)
{
	float H = normalize(V + L);

	float NoL = saturate(dot(N, L));
    float NoV = saturate(dot(N, V));
    float VoH = saturate(dot(V, H));
    float NoH = saturate(dot(N, H));

	float a2 = Pow4(Roughness);
	float D = GGX(a2, NoH);
	float F = FresnelSchlick(F0, VoH);
	float G = GeometrySchlickGGX(Roughness, NoV) * GeometrySchlickGGX(Roughness, NoL);

   	return (D * G * F) / (4 * max(NoL * NoV, 0.01f)); // 0.01 is added to prevent division by 0
}

float3 DefaultBRDF(float3 LightDir, float3 Normal, float3 ViewDir, float Roughness, float Metallic, float3 BaseColor)
{
	float3 F0 = lerp(F0_DIELECTRIC.rrr, BaseColor.rgb, Metallic);

	// Base color remapping
	float3 DiffuseColor = (1.0f - Metallic) * BaseColor;

	float3 DiffuseBRDF = LambertDiffuse(DiffuseColor);
    float3 SpecularBRDF = SpecularGGX(Normal, LightDir, ViewDir, Roughness, F0); 

	return DiffuseBRDF * SpecularBRDF;
}


float3 DirectLighting(float3 Radiance, float3 LightDir, float3 Normal, float3 ViewDir, float Roughness, float Metallic, float BaseColor,  float ShadowFactor)
{
	float3 BRDF = DefaultBRDF(LightDir, Normal, ViewDir, Roughness, Metallic, BaseColor);
	float NoL = saturate(dot(Normal, LightDir));
	return Radiance  * BRDF * NoL * ShadowFactor;
}

#endif