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


PixelOutput PS(VertexOut pin)
{
	PixelOutput Out;
	//float3 diffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, pin.TexC);
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 LightVec = -DirectLights[0].Direction;
	float NdotL = max(dot(LightVec, pin.Normal), 0.0f);
	Out.BaseColor = float4(float3(NdotL, NdotL, NdotL) , 1.0f);
	//Out.BaseColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return Out;
}
