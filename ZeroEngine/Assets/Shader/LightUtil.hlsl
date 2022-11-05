#ifndef LIGHT_HLSL
#define LIGHT_HLSL

#define MaxDirectLights 4

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 SchlickFresnel(float3 R0, float3 Normal, float3 LightVec)
{
    float CosIncidentAngle = saturate(dot(Normal, LightVec));

    float F0 = 1.0f - CosIncidentAngle;
    float3 ReflectPercent = R0 + (1.0f - R0)*(F0*F0*F0*F0*F0);

    return ReflectPercent;
}


float3 BlinnPhong(float3 LightIntensity, float3 LightVec, float3 Normal, float3 ToEye, FMaterial Mat)
{
    const float m = Mat.Shininess * 256.0f;
    float3 HalfVec = normalize(ToEye + LightVec);

    float RoughnessFactor = (m + 8.0f)*pow(max(dot(HalfVec, Normal), 0.0f), m) / 8.0f;
    float3 FresnelFactor = SchlickFresnel(Mat.FresnelR0, HalfVec, LightVec);

    float3 SpecAlbedo = FresnelFactor * RoughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    SpecAlbedo = SpecAlbedo / (SpecAlbedo + 1.0f);

    return (Mat.DiffuseAlbedo.rgb + SpecAlbedo) * LightIntensity;
}


float3 ComputeDirectionalLight(FDirectLight Light, FMaterial Mat, float3 Normal, float3 ToEye)
{
    // The light vector aims opposite the direction the light rays travel.
    float3 LightVec = -Light.Direction;

    // Scale light down by Lambert's cosine law.
    float NdotL = max(dot(LightVec, Normal), 0.0f);
    float3 LightIntensity = Light.Intensity * NdotL;

    return BlinnPhong(LightIntensity, LightVec, Normal, ToEye, Mat);
}


float4 ComputeLighting(FDirectLight gLights[MaxDirectLights], FMaterial Mat,
                       float3 WorldPos, float3 Normal, float3 ToEye,
                       float3 ShadowFactor)
{	
	float3 Result = 0.0f;

    int i;
    for(i = 0; i < 4; ++i)
    {
        Result += ShadowFactor * ComputeDirectionalLight(gLights[i], Mat, Normal, ToEye);
    }

	return float4(Result, 0.0f);
}

#endif