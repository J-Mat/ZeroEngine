#include "../Common.hlsl"
#include "../PBRLighting.hlsl"

cbuffer cbMaterial : register(b3)
{
	float Roughness;
};

VertexOut VS(VertexIn Vin)
{
	VertexOut Vout;

	Vout.WorldPos = Vin.PosL;

	float4 PosW = mul(Model, float4(Vin.PosL, 1.0f));
	PosW.xyz += ViewPos;

	Vout.PosH = mul(ProjectionView, PosW).xyww;

	return Vout;
}


float4 PS(VertexOut Pin) : SV_TARGET
{
	float3 N = normalize(Pin.WorldPos); 
	float3 R = N;
    float3 V = R;

	const uint SAMPLE_COUNT = 1024u;
    float TotalWeight = 0.0;   
    float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);

	for (uint i = 0u ; i < SAMPLE_COUNT; ++i)
	{
		float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H  = ImportanceSampleGGX(Xi, N, Roughness).xyz;
        float3 L  = normalize(2.0 * dot(V, H) * H - V);
		
		float NdotL = saturate(dot(N, L));
		if (NdotL > 0.0f)
		{
			PrefilteredColor +=  gSkyboxMap.Sample(gSamAnisotropicWarp, L).rgb * NdotL;
			TotalWeight += NdotL;
		}
	}
	PrefilteredColor = PrefilteredColor / TotalWeight;

	return float4(PrefilteredColor, 1.0f);
}
