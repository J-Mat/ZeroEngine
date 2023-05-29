Texture2D<float4> SrcTexture : register(t0);
RWTexture2D<float4> DstTexture : register(u0);

cbuffer PrefixSumCB : register( b0 )
{
    uint TexSize;	// Texture level of source mip
}

[numthreads(1024, 1, 1)]
void CS(
	int3 DispatchThreadID : SV_DispatchThreadID,
    int3 GroupThreadID : SV_GroupThreadID)
{
	uint ID = DispatchThreadID.x;
	uint2 P0 = int2(ID * 2, GroupThreadID.x);
	uint2 P1 = int2(ID * 2 + 1, GroupThreadID.x);

	DstTexture[P0.yx] = SrcTexture[P0.xy];
    DstTexture[P1.yx] = SrcTexture[P1.xy];
	
	const uint Steps = uint(log2(1024) + 1);

    uint ReadID;
    uint WriteID;

    for (uint Step = 0; Step < Steps; ++Step)
    {
        uint Mask = (1U << Step) - 1;
        uint ReadID = ((ID >> Step) << (Step + 1)) + Mask;
        uint WriteID = ReadID + 1 + (ID & Mask);
		DstTexture[uint2(GroupThreadID.x, WriteID)] += DstTexture[uint2(GroupThreadID.x,ReadID)];
        GroupMemoryBarrierWithGroupSync();
    }	
}