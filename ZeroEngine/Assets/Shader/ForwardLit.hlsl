#include "Common.hlsl"


Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);
Texture2D gMetallicMap: register(t2);
Texture2D gRoughnessMap: register(t3);
Texture2D gEmissionMap: register(t4);
TextureCube IBLIrradianceMap : register(t5);

Texture2D _BrdfLUT : register(t6);

TextureCube IBLPrefilterMaps[5] : register(t7);

#include "./PBRLighting.hlsl"
cbuffer cbMaterial : register(b3)
{
	float Roughness_Slider;
};

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;
	Vout.Normal = vin.Normal;
	Vout.Tangent = vin.Tangent;

	return Vout;
};



PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 FinalColor = 0.0f;

	float2 UV = float2(Pin.TexC.x, 1.0f - Pin.TexC.y);
	float3 Albedo = gDiffuseMap.Sample(gSamAnisotropicWarp, UV).rgb * Roughness_Slider; 
	float3 EmissiveColor = gEmissionMap.Sample(gSamAnisotropicWarp, UV).rgb;
	float Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, UV).r; 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, UV).r;
	//float AO = gAOMap.Sample(gSamAnisotropicWarp, UV).r;
	float3 NormalMap = gNormalMap.Sample(gSamAnisotropicWarp, UV).xyz;

	float3 N = NormalSampleToWorldSpace(NormalMap, Pin.Normal, Pin.Tangent);

	float3 ViewDir = normalize(ViewPos - Pin.WorldPos);
	float3 ReflectDir = reflect(-ViewDir, Pin.Normal);
	float3 PrefilteredColor  = GetPrefilteredColor(Roughness, ReflectDir);
	float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, N).rgb;
	float NdotV = dot(N, ViewDir);
	float2 LUT = _BrdfLUT.Sample(gSamAnisotropicWarp, float2(NdotV, Roughness)).rg;
	FinalColor = AmbientLighting(Metallic, Albedo, Irradiance, PrefilteredColor, LUT) + EmissiveColor;

    // gamma correct
    FinalColor = pow(FinalColor, 1.0f/2.2f); 

	Out.BaseColor = float4(FinalColor, 1.0f);
	return Out;
}