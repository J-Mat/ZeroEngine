#include "../Common.hlsl"

Texture2D gShadowMap: register(t0);

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

    // Already in homogeneous clip space.
    vout.PosH = float4(vin.PosL, 1.0f);
	
	vout.TexC = vin.TexC;
	
    return vout;
};


float4 PS(VertexOut pin) : SV_Target
{
	return float4(gShadowMap.Sample(gSamLinearWarp, pin.TexC).rrr, 1.0f);
}
