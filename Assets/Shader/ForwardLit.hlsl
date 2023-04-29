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
	float3 Albedo = gDiffuseMap.SampleLevel(gSamLinearWarp, UV, 0).rgb;
	float3 EmissiveColor = pow(gEmissionMap.Sample(gSamAnisotropicWarp, UV).rgb, 2.2f);
	float Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, UV).r; 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, UV).r;
		//float AO = gAOMap.Sample(gSamAnisotropicWarp, UV).r;
	float3 NormalMap = gNormalMap.Sample(gSamAnisotropicWarp, UV).xyz;
	//float3 N = NormalSampleToWorldSpace(NormalMap, Pin.Normal, Pin.Tangent);
	float3 N = float3(Pin.Normal.x, Pin.Normal.y, Pin.Normal.z);

	if (ShadingMode == 0)
	{
		float3 ViewDir = normalize(ViewPos - Pin.WorldPos.xyz);
		float3 ReflectDir = reflect(-ViewDir, N);
		float3 PrefilteredColor  = GetPrefilteredColor(IBLPrefilterMap, MipLevel, ReflectDir);
		float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, N).rgb;
		float NdotV = dot(N, ViewDir);
		float2 LUT = _BrdfLUT.Sample(gSamAnisotropicWarp, float2(NdotV, Roughness)).rg;
		FinalColor =  AmbientLighting(Metallic, Albedo, Irradiance, PrefilteredColor, LUT) + EmissiveColor;
	}
	else if (ShadingMode == 1)
	{
		float3 Ambient = 0.05f * Albedo;
		float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
		float3 L = normalize(-DirectLights[0].Direction);
		float3 V = normalize(ViewPos - Pin.WorldPos); 
		float3 H = normalize(V + L);
		float NdotL = max(0.0f, dot(Pin.Normal, L));
		
		float3 Diffuse = NdotL * LightColor;

		float3 Spec = 0.0f;
		float3 ReflectDir = reflect(-V, N);
		Spec = pow(max(dot(V, ReflectDir), 0.0f), 35.0f) * LightColor;

		float ShadowFactor = CalcShadowFactor(0, Pin.ShadowPosH, MipLevel);
		FinalColor =  Ambient + (Diffuse + Spec) * ShadowFactor * Albedo;
		//FinalColor =  float3(ShadowFactor, ShadowFactor, ShadowFactor); //Ambient + (Diffuse + Spec) * ShadowFactor * Albedo;
	}

    // gamma correct
    FinalColor = pow(FinalColor,    1.0f/2.2f); 

	Out.BaseColor = float4(FinalColor, 1.0f);
	return Out;
}