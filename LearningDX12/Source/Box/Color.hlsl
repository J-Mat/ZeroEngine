cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
}

struct VertexIn
{
	 float3 PosL : POSITION;
	 float3 Color : COLOR;
};

struct VertexOut
{
	float4 PosH : SV_Position;
	float4 Color : COLOR; 
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	vout.Color = vin.Color;

	return vout;
};

float PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
