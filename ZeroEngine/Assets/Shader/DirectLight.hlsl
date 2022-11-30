#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_Position;
	float2 TexC    : TEXCOORD;
    float3 Normal  : NORMAL;
};

struct PixelOutput
{
    float4 BaseColor    : SV_TARGET0;
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
	Out.BaseColor = float4(DirectLights[0].Color * DirectLights[0].Intensity, 1.0f);
	//Out.BaseColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
	return Out;
}
