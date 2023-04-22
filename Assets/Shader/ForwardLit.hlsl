#include "Common.hlsl"


Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);
Texture2D gMetallicMap: register(t2);
Texture2D gRoughnessMap: register(t3);
Texture2D gEmissionMap: register(t4);
TextureCube IBLIrradianceMap : register(t5);

Texture2D _BrdfLUT : register(t6);

TextureCube IBLPrefilterMap : register(t7);


Texture2D _gShadowMap : register(t0, space1);

#include "./PBRLighting.hlsl"
//#include "./Shadow/ShdowUtils.hlsl"


cbuffer cbMaterial : register(b3)
{
	float MipLevel;
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


#define PCF_SAMPLE_PIXLE_RADIUS    2
#define PCF_SAMPLE_COUNT           6

float2 ComputeDepthDerivative(float3 projCoords)
{
	float2 ddist_duv = 0.0f;
    
	// Packing derivatives of u,v, and distance to light source w.r.t. screen space x, and y
	float3 duvdist_dx = ddx(projCoords);
	float3 duvdist_dy = ddy(projCoords);
	
	// Invert texture Jacobian and use chain rule to compute ddist/du and ddist/dv
	// |ddist/du| = |du/dx du/dy|-T * |ddist/dx|
	// |ddist/dv| |dv/dx dv/dy| |ddist/dy|

	// Multiply ddist/dx and ddist/dy by inverse transpose of Jacobian
	float invDet = 1 / ((duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x));

	// Top row of 2x2
	ddist_duv.x = duvdist_dy.y * duvdist_dx.z; 
	ddist_duv.x -= duvdist_dx.y * duvdist_dy.z; 

	// Bottom row of 2x2
	ddist_duv.y = duvdist_dx.x * duvdist_dy.z; 
	ddist_duv.y -= duvdist_dy.x * duvdist_dx.z; 
	ddist_duv *= invDet;
    
	return ddist_duv;
}

float CalcShadowFactor(float4 ShadowPos)
{
	/*
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
	*/
	float4 ProjCoords = ShadowPos;
	ProjCoords.xyz /= ProjCoords.w;

    float2 ShadowTexCoord = 0.5f * ProjCoords.xy + 0.5f;
	ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

	float3 ReceiverPos = float3(ShadowTexCoord.xy, ProjCoords.z);
	float2 ddist_duv = ComputeDepthDerivative(ReceiverPos);
    uint Width, Height, NumMips;
    _gShadowMap.GetDimensions(0, Width, Height, NumMips);

    // Texel size.
    float dx = 1.0f / (float)Width;
    float UVRadius = PCF_SAMPLE_PIXLE_RADIUS * dx; 
	const int SampleCount = PCF_SAMPLE_COUNT;
	float Visibility = 0.0f;
    for (int i = 0; i < SampleCount; i++)
    {
		float2 UVOffset = (Hammersley(i, SampleCount) * 2.0f - 1.0f) * UVRadius;
		float2 SampleUV = ReceiverPos.xy + UVOffset;
		
		const float FixedBias = 0.003f;
		float ReceiverDepthBias = ReceiverPos.z + dot(ddist_duv, UVOffset) - FixedBias;
		
		Visibility += _gShadowMap.SampleCmpLevelZero(gSamShadow, SampleUV, ReceiverDepthBias).r;
	}
    
	return Visibility / SampleCount;
}


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 FinalColor = 0.0f;
	float2 UV = float2(Pin.TexC.x, 1.0f - Pin.TexC.y);
	float3 Albedo = gDiffuseMap.SampleLevel(gSamLinearWarp, UV, 0).rgb;
	float3 EmissiveColor = pow(gEmissionMap.Sample(gSamAnisotropicWarp, UV).rgb, 2.2f);
	float Metallic = gMetallicMap.Sample(gSamAnisotropicWarp, UV).r; 
	float Roughness = gRoughnessMap.Sample(gSamAnisotropicWarp, UV).r;
		//float AO = gAOMap.Sample(gSamAnisotropicWarp, UV).r;
	float3 NormalMap = gNormalMap.Sample(gSamAnisotropicWarp, UV).xyz;
	//float3 N = NormalSampleToWorldSpace(NormalMap, Pin.Normal, Pin.Tangent);
	float3 N = float3(Pin.Normal.x, Pin.Normal.y, Pin.Normal.z);

	if (ShadingMode == 0)
	{
		float3 ViewDir = normalize(ViewPos - Pin.WorldPos.xyz);
		float3 ReflectDir = reflect(-ViewDir, N);
		//float3 PrefilteredColor  = GetPrefilteredColor(MipLevel, ReflectDir);


		float Level = MipLevel * (IBL_PREFILTER_ENVMAP_MIP_LEVEL - 1);   
    	float3 PrefilteredColor = IBLPrefilterMap.SampleLevel(gSamLinearClamp, ReflectDir, Level).rgb;

		float3 Irradiance = IBLIrradianceMap.Sample(gSamLinearClamp, N).rgb;
		float NdotV = dot(N, ViewDir);
		float2 LUT = _BrdfLUT.Sample(gSamAnisotropicWarp, float2(NdotV, Roughness)).rg;
		FinalColor =  PrefilteredColor; //PrefilteredColor;//AmbientLighting(Metallic, Albedo, Irradiance, PrefilteredColor, LUT) + EmissiveColor;
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

		float3 Spec = 0.0f;
		float3 ReflectDir = reflect(-V, N);
		Spec = pow(max(dot(V, ReflectDir), 0.0f), 35.0f) * LightColor;

		float ShadowFactor = CalcShadowFactor(Pin.ShadowPosH);
		FinalColor =  Ambient + (Diffuse + Spec) * ShadowFactor * Albedo;
		//FinalColor =  float3(ShadowFactor, ShadowFactor, ShadowFactor); //Ambient + (Diffuse + Spec) * ShadowFactor * Albedo;
		//FinalColor = Albedo;
	}

    // gamma correct
    FinalColor = pow(FinalColor,    1.0f/2.2f); 

	Out.BaseColor = float4(FinalColor, 1.0f);
	return Out;
}