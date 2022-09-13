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
}

cbuffer cbMaterial : register(b2)
{
	float4  Color;
};

struct VertexIn
{
	 float3 PosL : POSITION;
	 float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH : SV_Position;
	float4 Color : COLOR; 
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(View, float4(vin.PosL, 1.0f));
	vout.PosH = mul(Projection, vout.PosH);
	
	vout.Color = vin.Color;

	return vout;
};

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
