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
	VertexOut vout;
	
	vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	vout.PosH = mul(View, vout.PosH);
	vout.PosH = mul(Projection, vout.PosH);
	
	vout.TexC = vin.TexC;
	vout.Normal = vin.Normal;

	return vout;
};

PixelOutput PS(VertexOut pin)
{
	PixelOutput Out;
	//float3 diffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, pin.TexC);
	Out.BaseColor = float4(pin.Normal, 1.0f);
	//Out.BaseColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return Out;
}
