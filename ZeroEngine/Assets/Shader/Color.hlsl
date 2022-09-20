SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWarp : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWarp : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);

Texture2D gDiffuseMap: register(t0);//所有漫反射贴图

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
	float4 Base;
};


cbuffer cbConstBuffer : register(b3)
{
	float4 Color;
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
	float2 TexC    : TEXCOORD;
    float3 Normal  : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(View, float4(vin.PosL, 1.0f));
	vout.PosH = mul(Projection, vout.PosH);
	
	vout.TexC = vin.TexC;
	vout.Normal = vin.Normal;

	return vout;
};

float4 PS(VertexOut pin) : SV_Target
{
	float3 diffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, pin.TexC);
	return float4(pin.Normal, 1.0f);
}
