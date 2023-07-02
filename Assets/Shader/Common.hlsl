#ifndef COMMON_HLSL
#define COMMON_HLSL


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
	float3 WorldPos : POSITION0;
    float4 ShadowPosH : POSITION1;
	float2 TexC    : TEXCOORD;
    float3 Normal  : NORMAL;
	float3 Tangent : TANGENT;
};

struct PixelOutput
{
    float4 BaseColor   : SV_TARGET0;
    float4 Velocity    : SV_TARGET1;
};

struct FDirectLight
{
    float3 Color;
    float Intensity;
    float3 Direction;
    float Blank;
    float4x4 ProjectionView;
};

struct FPointLight
{
    float3 Color;
    float Intensity;
    float3 LightPos;
    float Range;
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
    int ShadingMode;
}

cbuffer cbCameraObject : register(b1)
{
    float4x4 View;
    float4x4 Projection;
    float4x4 ProjectionView;
    float3 ViewPos;
    float NearZ;
    float FarZ;
}


cbuffer cbConstant : register(b2)
{
    FDirectLight DirectLights[4];
    FPointLight PointLights[4];
    int DirectLightNum;
    int PointLightNum;
};



float3 NormalSampleToWorldSpace(float3 NormalMapSample, float3 UnitNormalW, float3 TangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 NormalT = 2.0f * NormalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = UnitNormalW;
	float3 T = normalize(TangentW - dot(TangentW, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 BumpedNormalW = mul(NormalT, TBN);

	return normalize(BumpedNormalW);
}

#endif