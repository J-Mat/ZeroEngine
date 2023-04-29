#include "../Common.hlsl"
#include "../Sampler.hlsl"

Texture2D _gShadowMap: register(t0);

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
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	//return float4(pin.TexC, 0.0f, 1.0f);
	return float4(_gShadowMap.Sample(gSamLinearWarp, pin.TexC).rrr, 1.0f);
}
