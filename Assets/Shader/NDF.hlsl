
#include "Common.hlsl"
#include "LightUtil.hlsl"


Texture2D gDiffuseMap: register(t0);

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


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float4 DiffuseAlbedo = gDiffuseMap.Sample(gSamAnisotropicWarp, Pin.TexC);
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 L = normalize(-DirectLights[0].Direction);
	float3 V = normalize(ViewPos - Pin.WorldPos.xyz); 
	float3 N = normalize(Pin.Normal);
	float3 H = normalize(V + L);
	float NDF = DistributionGGX(N, H, Rougness);
	Out.BaseColor = float4(float3(NDF, NDF, NDF), 1.0f) + DiffuseAlbedo;
	//Out.BaseColor = pow(Out.BaseColor, 1.0/2.2); 
	//Out.BaseColor = float4(float3(NDF, NDF, NDF), 1.0f);
	//Out.BaseColor = float4(float3(Rougness, Rougness, Rougness), 1.0f);
	return Out;
}
