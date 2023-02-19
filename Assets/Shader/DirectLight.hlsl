#include "Common.hlsl"


cbuffer cbMaterial : register(b3)
{
	int DirectLightIndex;	
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
	//float3 diffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, pin.TexC);
	Out.BaseColor = float4(DirectLights[DirectLightIndex].Color * DirectLights[DirectLightIndex].Intensity, 1.0f);
	//Out.BaseColor = float4(DirectLights[1].Color * DirectLights[1].Intensity, 1.0f);
	//Out.BaseColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
	return Out;
}
