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
	float3 WorldPos : POSITION;
};

struct PixelOutput
{
    float4 BaseColor    : SV_TARGET0;
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
	float4 DiffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, Pin.TexC);
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 LightVec = -DirectLights[0].Direction;
	float NdotL = max(dot(LightVec, Pin.Normal), 0.0f);
	//Out.BaseColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	float x = clamp(Pin.WorldPos.x, -1.0f, 1.0f);
	float y = clamp(Pin.WorldPos.y, -1.0f, 1.0f);
	float z = clamp(Pin.WorldPos.z, -1.0f, 1.0f);
	float3 test = float3(x,y,z);
	//Out.BaseColor = gSkyboxMap.Sample(gSamLinearClamp, float3(x, y, z));
	Out.BaseColor = float4(DiffuseAlbedo.xyz, 1.0f);
	//Out.BaseColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return Out;
}
