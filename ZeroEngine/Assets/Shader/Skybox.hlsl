SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWarp : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWarp : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);

TextureCube gSkyboxMap : register(t0);

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
	float3 PosL : POSITION;
};


VertexOut VS(VertexIn Vin)
{
	VertexOut Vout;

	Vout.PosL = Vin.PosL;

	float4 PosW = mul(Model, float4(Vin.PosL, 1.0f));
	PosW.xyz += ViewPos;

	Vout.PosH = mul(ProjectionView, PosW).xyww;

	return Vout;
};

struct PixelOutput
{
    float4 BaseColor    : SV_TARGET0;
};

PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
    Out.BaseColor = gSkyboxMap.Sample(gSamLinearWarp, Pin.PosL);
	return Out;
}
