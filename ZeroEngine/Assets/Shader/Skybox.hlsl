#include "Common.hlsl"

VertexOut VS(VertexIn Vin)
{
	VertexOut Vout;

	Vout.WorldPos = Vin.PosL;

	float4 PosW = mul(Model, float4(Vin.PosL, 1.0f));
	PosW.xyz += ViewPos;

	Vout.PosH = mul(ProjectionView, PosW).xyww;

	return Vout;
};

PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
    Out.BaseColor = gSkyboxMap.Sample(gSamLinearWarp, Pin.WorldPos);
	return Out;
}
