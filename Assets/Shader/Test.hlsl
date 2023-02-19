#include "Common.hlsl"

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	Vout.PosH = float4(vin.PosL, 1.0f);
	return Vout;
};


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	Out.BaseColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	return Out;
}
