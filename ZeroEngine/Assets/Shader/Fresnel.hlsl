#include "Common.hlsl"											

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.Normal = vin.Normal;

	return Vout;
};


float3 SchlickFresnel(float3 F0, float3 Normal, float3 LightVec)
{
    float CosTheta = saturate(dot(Normal, LightVec));
    return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * pow(1.0f - CosTheta, 5.0f);
}

PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 LightVec = -DirectLights[0].Direction;
	float NdotL = saturate(dot(Pin.Normal, LightVec));

	//float3 Fresnel = float3(Fresnel_1, Fresnel_2, Fresnel_3);
	//float3 Color = 	SchlickFresnel(Fresnel, Pin.Normal, LightVec);
	Out.BaseColor = float4(float3(NdotL, NdotL, NdotL), 1.0f);
	return Out;
}
