#include "Common.hlsl"
#include "Sampler.hlsl"

TextureCube gSkyboxMap : register(t0);

VertexOut VS(VertexIn Vin)
{
	VertexOut Vout;

    Vout.WorldPos = float4(Vin.PosL, 1.0f);

	float4 PosW = mul(Model, float4(Vin.PosL, 1.0f));
	PosW.xyz += ViewPos;

	Vout.PosH = mul(ProjectionView, PosW).xyww;

	return Vout;
};

float4 PS(VertexOut Pin) : SV_TARGET
{
    float3 BaseColor = gSkyboxMap.Sample(gSamLinearWarp, Pin.WorldPos.xyz);
	BaseColor = pow(BaseColor, 1.0f/2.2f); 
	return float4(BaseColor, 1.0f);
}
