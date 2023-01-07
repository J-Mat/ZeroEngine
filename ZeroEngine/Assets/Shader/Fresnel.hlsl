#include "Common.hlsl"											
#include "LightUtil.hlsl"

cbuffer cbMaterial : register(b3)
{
	float Fresnel_1;
	float Fresnel_2;
	float Fresnel_3;
};


VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.Normal = vin.Normal;

	return Vout;
};


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 N = normalize(Pin.Normal);
	float3 V = normalize(ViewPos - Pin.WorldPos);
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 LightVec = -DirectLights[0].Direction;
	float NdotL = saturate(dot(Pin.Normal, LightVec));

	float3 Fresnel = float3(Fresnel_1, Fresnel_2, Fresnel_3);
	float F = FresnelSchlick(max(dot(N, V), 0.0f), Fresnel);
	Out.BaseColor = float4(float3(F, F, F), 1.0f);
	return Out;
}
