#ifndef COMMON_HLSL
#define COMMON_HLSL

SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWarp : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWarp : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);

struct FDirectLight
{
    float3 Color;
    float Intensity;
    float4x4 ProjView;
    float3 Direction;
};


struct FMaterial
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};


cbuffer cbPerObject : register(b0)
{
    float4x4 Model;
}

cbuffer cbCameraObject : register(b1)
{
    float4x4 View;
    float4x4 Projection;
    float4x4 ProjectionView;
    float3 ViewPos;
}

cbuffer cbMaterial : register(b2)
{
    float4 Base;
};

cbuffer cbConstant : register(b3)
{
    FDirectLight DirectLights[4];
    int DirectLightNum;
};

Texture2D gDiffuseMap: register(t0);
TextureCube gSkyboxMap : register(t1);

//#include "LightUtil.hlsl"

#endif