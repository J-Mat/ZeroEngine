Texture2D<float4> SrcTexture : register(t0);
RWTexture2D<float4> DstTexture : register(u0);

[numthreads(8, 8, 1)]
void CS(int3 DispatchThreadID : SV_DispatchThreadID,
        int3 GroupThreadID : SV_GroupThreadID)
{
    float4 Color = SrcTexture[DispatchThreadID.xy];
    DstTexture[DispatchThreadID.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
}