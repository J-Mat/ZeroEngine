#include "Common.hlsl"

cbuffer cbMaterial : register(b3)
{
    float test;
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
	float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, Pin.Normal).rgb;
	Out.BaseColor = float4(Irradiance, 1.0f);
	return Out;
}
