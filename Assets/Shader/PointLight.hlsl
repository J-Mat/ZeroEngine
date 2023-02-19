#include "Common.hlsl"


cbuffer cbMaterial : register(b3)
{
	int PointLightIndex;	
};


VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;
	Vout.Normal = vin.Normal;

	return Vout;
};


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	Out.BaseColor = float4(PointLights[PointLightIndex].Color * PointLights[PointLightIndex].Intensity, 1.0f);
	return Out;
}
