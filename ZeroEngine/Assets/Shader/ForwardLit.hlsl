#include "Common.hlsl"
#include "./PBRLighting.hlsl"

cbuffer cbMaterial : register(b3)
{
	float Roughness;
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

	return Vout;
};


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 ViewDir = normalize(ViewPos - Pin.WorldPos);
	float3 ReflectDir = reflect(-ViewDir, Pin.Normal);
	float3 PrefilteredColor  = GetPrefilteredColor(Roughness, ReflectDir);
    //float3 FloorSample = IBLPrefilterMaps[0].Sample(gSamLinearClamp, ReflectDir).rgb;
	//PrefilteredColor  = gSkyboxMap.Sample(gSamLinearWarp, ReflectDir);
	//float3 t = Test[2].Sample(gSamAnisotropicWarp, Pin.TexC).rgb;
	Out.BaseColor = float4(PrefilteredColor, 1.0f);
	return Out;
}
