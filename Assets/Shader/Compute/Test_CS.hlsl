
Texture2D<float4> SrcTexture : register(t0);
RWTexture2D<float4> DstTexture : register(u0);

cbuffer PassCB : register(b0)
{
	float2 texelSize;
}

// Linear clamp sampler.
SamplerState LinearClampSampler : register( s0 );

[numthreads(8, 8, 1)]
void CS(uint3 DTid : SV_DispatchThreadID)
{
	float2 uv = (DTid.xy + 0.5) * texelSize;
	DstTexture[DTid.xy] = SrcTexture.SampleLevel(LinearClampSampler, uv, 0);
}

