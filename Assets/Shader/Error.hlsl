#include "Common.hlsl"

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);

	return Vout;
};


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	Out.BaseColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	return Out;
}
