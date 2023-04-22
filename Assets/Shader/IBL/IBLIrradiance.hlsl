#include "../Common.hlsl"

TextureCube gSkyboxMap : register(t0);

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
	float3 Irradiance = float3(0.0f, 0.0f, 0.0f);

	// The sample direction equals the hemisphere's orientation
	float3 Normal = normalize(Pin.WorldPos.xyz);
	float3 Up = float3(0.0, 1.0, 0.0);
	float3 Right = cross(Up, Normal);
	Up = cross(Normal, Right);

	float SampleDelta = 0.025f;
	float SampleCount = 0.0f;
	for (float phi = 0.0f; phi < 2.0f * PI; phi += SampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += SampleDelta)
		{
			// Spherical to cartesian (in tangent space)
			float3 TangentSample = float3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// Tangent space to world
			float3 SampleVec = TangentSample.x * Right + TangentSample.y * Up + TangentSample.z * Normal;
			
			Irradiance += gSkyboxMap.Sample(gSamAnisotropicWarp, SampleVec).rgb * cos(theta) * sin(theta);
			SampleCount++;
		}
	}
	Irradiance = PI * Irradiance * (1.0f / SampleCount);

	return float4(Irradiance, 1.0f);
}
