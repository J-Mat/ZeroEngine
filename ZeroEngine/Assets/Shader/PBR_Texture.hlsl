
#include "Common.hlsl"
#include "LightUtil.hlsl"


VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    Vout.Normal = mul((float3x3)Model, vin.Normal);
    Vout.Tangent = mul((float3x3)Model, vin.Tangent);

	return Vout;
};

PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;

	float3 Albedo = gDiffuseMap.Sample(gSamAnisotropicWarp, Pin.TexC); 
	float3 Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, Pin.TexC); 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, Pin.TexC).r;
	float AO = gAOMap.Sample(gSamAnisotropicWarp, Pin.TexC).r;
	float3 Normal = gNormalMap.Sample(gSamAnisotropicWarp, Pin.TexC).xyz;
	
	float3 N = NormalSampleToWorldSpace(Normal, Pin.Normal, Pin.Tangent);
	float3 V = normalize(ViewPos - Pin.WorldPos);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, Albedo, Metallic);

	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < PointLightNum; ++i)
	{
		float3 L = normalize(PointLights[i].LightPos - Pin.WorldPos);
		float3 H = normalize(L + V);
		
		float Distance = length(PointLights[i].LightPos - Pin.WorldPos);
		float Attenuation = 1.0f / (Distance * Distance);
		float3 Radiance = PointLights[i].Color * PointLights[i].Intensity * Attenuation;

        // Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, Roughness);
		float G = GeometrySmith(N, V, L, Roughness);
		float3 F = FresnelSchlick(clamp(dot(N, V), 0.0f, 1.0f), F0);

		float3 Numerator = NDF * G * F;
		float Denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001; 
		float3 Specular = Numerator / Denominator;
		
		float3 Ks = F;
		float3 Kd = float3(1.0f, 1.0f, 1.0f) - Ks;
    	Kd *= 1.0f - Metallic;	

		float NdotL = max(dot(N, L), 0.0);   
		Lo += (Kd * Albedo / PI + Specular) * Radiance * NdotL;
	}

  	float3 Ambient = float3(0.03f, 0.03f, 0.03f) * Albedo * AO;
	
	float3 Color = Ambient + Lo;

	// HDR tonemapping
    Color = Color / (Color + float3(1.0f, 1.0f, 1.0f));
    // gamma correct
    Color = pow(Color, 1.0f/2.2f);
	
	Out.BaseColor = float4(Color, 1.0f);

	return Out;
}
