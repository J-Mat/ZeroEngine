
#ifndef PBRLIGHTING_HLSL
#define PBRLIGHTING_HLSL

#define IBL_PREFILTER_ENVMAP_MIP_LEVEL 5

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
float3 GetPrefilteredColor(float Roughness, float3 ReflectDir)
{
    float Level = Roughness * (IBL_PREFILTER_ENVMAP_MIP_LEVEL - 1);
    int FloorLevel = floor(Level);
    int CeilLevel = ceil(Level);
    
    float3 FloorSample = IBLPrefilterMaps[FloorLevel].Sample(gSamLinearClamp, ReflectDir).rgb;
	float3 CeilSample = IBLPrefilterMaps[CeilLevel].Sample(gSamLinearClamp, ReflectDir).rgb;

    float3 PrefilteredColor = lerp(FloorSample, CeilSample, (Level - FloorLevel));
    return PrefilteredColor;

}

float3 EnvBRDF(float Metallic, float3 BaseColor, float2 LUT)
{
	float3 F0_DIELECTRIC = float3(0.04f, 0.04f, 0.04f);
    float3 F0 = lerp(F0_DIELECTRIC, BaseColor, Metallic); 
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


#endif