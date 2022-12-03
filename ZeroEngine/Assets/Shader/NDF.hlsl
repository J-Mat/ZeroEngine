
struct FDirectLight
{
    float3 Color;
    float Intensity;
    float4x4 ProjView;
    float3 Direction;
};


cbuffer cbPerObject : register(b0)
{
    float4x4 Model;
}

cbuffer cbCameraObject : register(b1)
{
    float4x4 View;
    float4x4 Projection;
    float4x4 ProjectionView;
    float3 ViewPos;
}

cbuffer cbMaterial : register(b2)
{
	float Rougness;
};

cbuffer cbConstant : register(b3)
{
    FDirectLight DirectLights[4];
    int DirectLightNum;
};

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
	
	Vout.Normal = vin.Normal;

	return Vout;
};


float3 SchlickFresnel(float3 F0, float3 Normal, float3 LightVec)
{
    float CosTheta = saturate(dot(Normal, LightVec));
    return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * pow(1.0f - CosTheta, 5.0f);
}

#define PI 3.1415926

float DistributionGGX(float3 N, float3 H, float a)
{
	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;
	
	float a2 = a * a;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;
	
	return a2 / denom;
}



PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 L = normalize(-DirectLights[0].Direction);
	float3 V = normalize(ViewPos - Pin.WorldPos); 
	float3 H = normalize(V + L);
	float NdotL = saturate(dot(L, Pin.Normal));
	float NDF = DistributionGGX(Pin.Normal, H, Rougness);
	float NdotH = max(dot(Pin.Normal, H), 0.0f);
	Out.BaseColor = float4(float3(NDF, NDF, NDF), 1.0f);
	//Out.BaseColor = float4(float3(NDF, NDF, NDF), 1.0f);
	//Out.BaseColor = float4(float3(Rougness, Rougness, Rougness), 1.0f);
	return Out;
}
