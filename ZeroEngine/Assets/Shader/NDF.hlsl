
#include "Common.hlsl"
cbuffer cbMaterial : register(b3)
{
	float Rougness;
};


VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;
	Vout.Normal = vin.Normal;

	return Vout;
};


float3 SchlickFresnel(float3 F0, float3 Normal, float3 LightVec)
{
    float CosTheta = saturate(dot(Normal, LightVec));
    return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * pow(1.0f - CosTheta, 5.0f);
}

#define PI 3.1415926

float DistributionGGX(float3 N, float3 H, float a)
{
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;	
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;
	
	return nom / denom;
}


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float4 DiffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, Pin.TexC);
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 L = normalize(-DirectLights[0].Direction);
	float3 V = normalize(ViewPos - Pin.WorldPos); 
	float3 N = normalize(Pin.Normal);
	float3 H = normalize(V + L);
	float NDF = DistributionGGX(N, H, Rougness);
	Out.BaseColor = float4(float3(NDF, NDF, NDF), 1.0f) + DiffuseAlbedo;
	//Out.BaseColor = pow(Out.BaseColor, 1.0/2.2); 
	//Out.BaseColor = float4(float3(NDF, NDF, NDF), 1.0f);
	//Out.BaseColor = float4(float3(Rougness, Rougness, Rougness), 1.0f);
	return Out;
}
