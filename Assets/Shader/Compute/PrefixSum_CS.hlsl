//Buffer<float> gInputA : register(t0);
RWBuffer<float> gOutput : register(u0);
  

[numthreads(32, 1, 1)]
void CS(int3 DispatchThreadID : SV_DispatchThreadID,
        int3 GroupThreadID : SV_GroupThreadID)
{
    uint ID = DispatchThreadID.x;
    gOutput[ID]= 12.0f;
    /*
    const uint Steps = uint(log2(1024) + 1);
    uint ReadID;
    uint WriteID;

    gOutput[ID * 2]= gInputA[ID * 2];
    gOutput[ID * 2 + 1] = gInputA[ID * 2 + 1];
    GroupMemoryBarrierWithGroupSync();
    for (uint Step = 0; Step < Steps; ++Step)
    {
        uint Mask = (1U << Step) - 1;
        uint ReadID = ((ID >> Step) << (Step + 1U)) + Mask;
        uint WriteID = ReadID + 1 + (ID & Mask);
        gOutput[WriteID] += gOutput[ReadID];
        GroupMemoryBarrierWithGroupSync();
    }
    */
}