#include "Common.hlsl"


Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);
Texture2D gMetallicMap: register(t2);
Texture2D gRoughnessMap: register(t3);
Texture2D gEmissionMap: register(t4);
TextureCube IBLIrradianceMap : register(t5);

Texture2D _BrdfLUT : register(t6);

TextureCube IBLPrefilterMaps[5] : register(t7);

Texture2D _gShadowMap : register(t0, space1);

#include "./PBRLighting.hlsl"
//#include "./Shadow/ShdowUtils.hlsl"


cbuffer cbMaterial : register(b3)
{
	float4x4 LightProjectionView;
};

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.TexC = vin.TexC;
	Vout.Normal = mul((float3x3)Model ,vin.Normal);
	Vout.Tangent = mul((float3x3)Model, vin.Tangent);

	
	Vout.ShadowPosH = mul(DirectLights[0].ProjectionView, float4(Vout.WorldPos, 1.0f));
	
	return Vout;
};

float CalcShadowFactor(float4 ShadowPos)
{
	float4 ProjCoords = ShadowPos;
	ProjCoords.xyz /= ProjCoords.w;

    float2 ShadowTexCoord = 0.5f * ProjCoords.xy + 0.5f;
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

    uint Width, Height, NumMips;
    _gShadowMap.GetDimensions(0, Width, Height, NumMips);

    // Texel size.
    float dx = 1.0f / (float)Width;

	float PercentLit = 0.0f;
    const float2 Offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

	float Depth = ProjCoords.z;

    [unroll]
    for(int i = 0; i < 9; ++i)
    {
        PercentLit += _gShadowMap.SampleCmpLevelZero(gSamShadow,
            ShadowTexCoord.xy + Offsets[i], Depth).r;
    }
    
    return PercentLit / 9.0f;
}


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 FinalColor = 0.0f;
	float2 UV = float2(Pin.TexC.x, 1.0f - Pin.TexC.y);
	float3 Albedo = pow(gDiffuseMap.Sample(gSamAnisotropicWarp, UV).rgb, 2.2f);
	float3 EmissiveColor = pow(gEmissionMap.Sample(gSamAnisotropicWarp, UV).rgb, 2.2f);
	float Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, UV).r; 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, UV).r;
		//float AO = gAOMap.Sample(gSamAnisotropicWarp, UV).r;
	float3 NormalMap = gNormalMap.Sample(gSamAnisotropicWarp, UV).xyz;
	float3 N = NormalSampleToWorldSpace(NormalMap, Pin.Normal, Pin.Tangent);

	if (ShadingMode == 0)
	{

		float3 ViewDir = normalize(ViewPos - Pin.WorldPos.xyz);
		float3 ReflectDir = reflect(-ViewDir, N);
		float3 PrefilteredColor  = GetPrefilteredColor(Roughness, ReflectDir);
		float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, N).rgb;
		float NdotV = dot(N, ViewDir);
		float2 LUT = _BrdfLUT.Sample(gSamAnisotropicWarp, float2(NdotV, Roughness)).rg;
		FinalColor = AmbientLighting(Metallic, Albedo, Irradiance, PrefilteredColor, LUT) + EmissiveColor;
	}
	else if (ShadingMode == 1)
	{
		float3 Ambient = 0.05f * Albedo;
		float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
		float3 L = normalize(-DirectLights[0].Direction);
		float3 V = normalize(ViewPos - Pin.WorldPos); 
		float3 H = normalize(V + L);
		float NdotL = max(0.0f, dot(Pin.Normal, L));
		
		float3 Diffuse = NdotL * LightColor;

		float Spec = 0.0f;
		float3 ReflectDir = reflect(-V, N);
		Spec = pow(max(dot(V, ReflectDir), 0.0f), 35.0f) * LightColor;

		float ShadowFactor = CalcShadowFactor(Pin.ShadowPosH);
		FinalColor =  Ambient + (Diffuse + Spec) * ShadowFactor * Albedo;
;
	}

    // gamma correct
    FinalColor = pow(FinalColor, 1.0f/2.2f); 

	Out.BaseColor = float4(FinalColor, 1.0f);
	return Out;
}