#include "Common.hlsl"
#include "Sampler.hlsl"


Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);
Texture2D gMetallicMap: register(t2);
Texture2D gRoughnessMap: register(t3);
Texture2D gEmissionMap: register(t4);
TextureCube IBLIrradianceMap : register(t5);

Texture2D _BrdfLUT : register(t6);

TextureCube IBLPrefilterMap : register(t7);

TextureCube _gShadowMapCube : register(t8);
Texture2D _gShadowMaps[2] : register(t0, space1);

#include "./Utils.hlsl"
#include "./Shadow/ShadowUtils.hlsl"

cbuffer cbMaterial : register(b3)
{
	float MipLevel;
};

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;
	Vout.Normal = mul((float3x3)Model ,vin.Normal);
	Vout.Tangent = mul((float3x3)Model, vin.Tangent);

	
	Vout.ShadowPosH = mul(DirectLights[0].ProjectionView, float4(Vout.WorldPos, 1.0f));
	
	return Vout;
};


#define DIRECTIONAL_LIGHT_PIXEL_WIDTH       10.0f
#define SPOT_LIGHT_PIXEL_WIDTH              10.0f


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 FinalColor = 0.0f;
	float2 UV = float2(Pin.TexC.x, 1.0f - Pin.TexC.y);
	float3 BaseColor = gDiffuseMap.SampleLevel(gSamLinearWarp, UV, 0).rgb;
	float3 EmissiveColor = pow(gEmissionMap.Sample(gSamAnisotropicWarp, UV).rgb, 2.2f);
	float Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, UV).r; 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, UV).r;
		//float AO = gAOMap.Sample(gSamAnisotropicWarp, UV).r;
	float3 NormalMap = gNormalMap.Sample(gSamAnisotropicWarp, UV).xyz;
	//float3 N = NormalSampleToWorldSpace(NormalMap, Pin.Normal, Pin.Tangent);
	float3 Normal = Pin.Normal;

	float3 ViewDir = normalize(ViewPos - Pin.WorldPos.xyz);

	if (ShadingMode == 0)
	{
		float3 ReflectDir = reflect(-ViewDir, Normal);
		float3 PrefilteredColor  = GetPrefilteredColor(IBLPrefilterMap, MipLevel, ReflectDir);
		float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, Normal).rgb;
		float NdotV = dot(Normal, ViewDir);
		float2 LUT = _BrdfLUT.Sample(gSamAnisotropicWarp, float2(NdotV, Roughness)).rg;
		FinalColor =  AmbientLighting(Metallic, BaseColor, Irradiance, PrefilteredColor, LUT) + EmissiveColor;
	}
	else if (ShadingMode == 1)
	{
		float3 Ambient = 0.05f * BaseColor;
		FinalColor = Ambient;
		for (int LightIndex = 0; LightIndex < DirectLightNum; ++LightIndex)
		{
			float3 LightColor = DirectLights[LightIndex].Color * DirectLights[LightIndex].Intensity;
			float3 L = normalize(-DirectLights[LightIndex].Direction);
			float3 V = normalize(ViewPos - Pin.WorldPos); 
			float3 H = normalize(V + L);
			float NdotL = max(0.0f, dot(Pin.Normal, L));
			
			float3 Diffuse = NdotL * LightColor;

			float3 Spec = 0.0f;
			float3 ReflectDir = reflect(-V, Normal);
			Spec = pow(max(dot(V, ReflectDir), 0.0f), 35.0f) * LightColor;

			float ShadowFactor = CalcShadowFactor(0, Pin.ShadowPosH, MipLevel);
			FinalColor +=  (Diffuse + Spec) * ShadowFactor * BaseColor;
		}
		
		if (PointLightNum > 0)
		{
			float3 LightToPoint = Pin.WorldPos - PointLights[0].LightPos;
			float ShadowFactor = CalcVisibilityOmni(LightToPoint, 0, 10.0f);
				
			float3 LightDir = normalize(PointLights[0].LightPos - Pin.WorldPos);
			float Attenuation = CalcDistanceAttenuation(Pin.WorldPos, PointLights[0].LightPos, PointLights[0].Range);
			float3 Radiance = PointLights[0].Intensity * Attenuation * PointLights[0].Color;
				//FinalColor += DirectLighting(Radiance, LightDir, Normal, ViewDir, Roughness, Metallic, BaseColor, ShadowFactor);

			float3 V = normalize(ViewPos - Pin.WorldPos); 
			float3 L = normalize(-LightToPoint);
			float NdotL = max(0.0f, dot(Pin.Normal, L));
			float3 Diffuse = NdotL * Radiance * Attenuation;

			float3 Spec = 0.0f;
			float3 ReflectDir = reflect(-V, Normal);
			Spec = pow(max(dot(V, ReflectDir), 0.0f), 35.0f) * Radiance * Attenuation;
			FinalColor +=  (Diffuse + Spec) * ShadowFactor * BaseColor;
		}

			//float ShadowFactor = CalcShadowFactor(0, Pin.ShadowPosH, MipLevel);

			//FinalColor +=  (Diffuse + Spec) * ShadowFactor * BaseColor;
		//FinalColor =  float3(ShadowFactor, ShadowFactor, ShadowFactor); //Ambient + (Diffuse + Spec) * ShadowFactor * BaseColor;
	}

    // gamma correct
    FinalColor = pow(FinalColor,    1.0f/2.2f); 

	Out.BaseColor = float4(FinalColor, 1.0f);
	return Out;
}