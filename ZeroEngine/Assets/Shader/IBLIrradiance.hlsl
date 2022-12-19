#include "Common.hlsl"

//TextureCube EnvironmentMap;

VertexOut VS(VertexIn Vin)
{
	VertexOut Vout;

	Vout.WorldPos = Vin.PosL;

	float4 PosW = mul(Model, float4(Vin.PosL, 1.0f));
	PosW.xyz += ViewPos;

	Vout.PosH = mul(Model, float4(Vin.PosL, 1.0f));
	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);

	return Vout;
}


float4 PS(VertexOut Pin) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
