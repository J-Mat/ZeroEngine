#include "Common.hlsl"


Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);
Texture2D gMetallicMap: register(t2);
Texture2D gRoughnessMap: register(t3);
Texture2D gEmissionMap: register(t4);
TextureCube IBLIrradianceMap : register(t5);

Texture2D _BrdfLUT : register(t6);

Texture2D gShadowMap : register(t0, space1); 
TextureCube IBLPrefilterMaps[5] : register(t8);


#include "./PBRLighting.hlsl"
//#include "./Shadow/ShdowUtils.hlsl"


cbuffer cbMaterial : register(b3)
{
	float4x4 LightProjectionView;
};

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.ShadowPosH = mul(LightProjectionView, Vout.PosH);
	Vout.WorldPos = Vout.PosH;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;
	Vout.Normal = mul((float3x3)Model ,vin.Normal);
	Vout.Tangent = mul((float3x3)Model, vin.Tangent);

	//if (DirectLightNum > 0)
	{
	}
	return Vout;
};

float CalcShadowFactor(float4 ShadowPos)
{
	// Complete projection by doing division by w.
	float4 ProjCoords = ShadowPos;
	ProjCoords.xyz /= ProjCoords.w;

    float2 ShadowTexCoord = 0.5f * ProjCoords.xy + 0.5f;
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
   	float ClosestDepth = gShadowMap.Sample(gSamLinearClamp, ShadowTexCoord).r;

   	float CurrentDepth = ProjCoords.z - 0.005f;

   	float Shadow = CurrentDepth > ClosestDepth  ? 1.0f : 0.0f;

    return Shadow;
}


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 FinalColor = 0.0f;
	float2 UV = float2(Pin.TexC.x, 1.0f - Pin.TexC.y);
	float3 Albedo = gDiffuseMap.Sample(gSamAnisotropicWarp, UV).rgb;
	float3 EmissiveColor = gEmissionMap.Sample(gSamAnisotropicWarp, UV).rgb;
	float Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, UV).r; 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, UV).r;
		//float AO = gAOMap.Sample(gSamAnisotropicWarp, UV).r;
	float3 NormalMap = gNormalMap.Sample(gSamAnisotropicWarp, UV).xyz;
	float3 N = NormalSampleToWorldSpace(NormalMap, Pin.Normal, Pin.Tangent);

	if (ShadingMode == 0)
	{

		float3 ViewDir = normalize(ViewPos - Pin.WorldPos.xyz);
		float3 ReflectDir = reflect(-ViewDir, N);
		float3 PrefilteredColor  = GetPrefilteredColor(Roughness, ReflectDir);
		float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, N).rgb;
		float NdotV = dot(N, ViewDir);
		float2 LUT = _BrdfLUT.Sample(gSamAnisotropicWarp, float2(NdotV, Roughness)).rg;
		FinalColor = AmbientLighting(Metallic, Albedo, Irradiance, PrefilteredColor, LUT) + EmissiveColor;
	}
	else if (ShadingMode == 1)
	{
		float3 Ambient = 0.6f;
		/*
		float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
		float3 L = normalize(-DirectLights[0].Direction);
		float3 V = normalize(ViewPos - Pin.WorldPos); 
		float3 H = normalize(V + L);
		float NdotL = max(0.0f, dot(Pin.Normal, L));
		
		float3 Diffuse = NdotL * LightColor * Albedo;

		float Spec = 0.0f;
		float3 ReflectDir = reflect(-V, N);
		Spec = pow(max(dot(V, ReflectDir), 0.0f), 35.0f);
		*/	
		float4 ShadowPos = mul(LightProjectionView, Pin.WorldPos); 
		float ShadowFactor = CalcShadowFactor(ShadowPos);
		float3 Specula = Ambient * (1.0f - ShadowFactor);

		FinalColor = Specula;
	}

    // gamma correct
    FinalColor = pow(FinalColor, 1.0f/2.2f); 

	Out.BaseColor = float4(FinalColor, 1.0f);
	return Out;
}