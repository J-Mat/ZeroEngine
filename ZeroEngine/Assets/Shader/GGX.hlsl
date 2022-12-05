
struct FDirectLight
{
    float3 Color;
    float Intensity;
    float4x4 ProjView;
    float3 Direction;
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
	float Rougness;
};

cbuffer cbConstant : register(b3)
{
    FDirectLight DirectLights[4];
    int DirectLightNum;
};

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
    float3 Normal  : NORMAL;
	float3 WorldPos : POSITION;
};

struct PixelOutput
{
    float4 BaseColor    : SV_TARGET0;
};


VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	Vout.Normal = vin.Normal;

	return Vout;
};


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}


PixelOutput PS(VertexOut Pin)
{
	PixelOutput Out;
	float3 LightColor = DirectLights[0].Color * DirectLights[0].Intensity;
	float3 L = normalize(-DirectLights[0].Direction);
	float3 V = normalize(ViewPos - Pin.WorldPos); 
	float3 N = normalize(Pin.Normal);
	float3 H = normalize(V + L);
	float GGX = GeometrySmith(N, V, L, Rougness);
	Out.BaseColor = float4(float3(GGX, GGX, GGX), 1.0f);
	//Out.BaseColor = pow(Out.BaseColor, 1.0/2.2); 
	return Out;
}
