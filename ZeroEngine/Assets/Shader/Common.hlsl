#ifndef COMMON_HLSL
#define COMMON_HLSL

SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWarp : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWarp : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);


Texture2D gDiffuseMap: register(t0);
Texture2D gNormalMap: register(t1);
Texture2D gMetallicMap: register(t2);
Texture2D gRoughnessMap: register(t3);
Texture2D gAOMap: register(t4);
TextureCube gSkyboxMap : register(t5);

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_Position;
	float2 TexC    : TEXCOORD;
    float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
	float3 WorldPos : POSITION;
};

struct PixelOutput
{
    float4 BaseColor    : SV_TARGET0;
};

struct FDirectLight
{
    float3 Color;
    float Intensity;
    float3 Direction;
    float Blank;
    float4x4 ProjView;
};

struct FPointLight
{
    float3 Color;
    float Intensity;
    float3 LightPos;
    float Blank;
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


cbuffer cbConstant : register(b2)
{
    FDirectLight DirectLights[4];
    FPointLight PointLights[4];
    int DirectLightNum;
    int PointLightNum;
};


#endif