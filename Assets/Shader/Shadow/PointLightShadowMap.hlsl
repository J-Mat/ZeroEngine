#include "../Common.hlsl"

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));

	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);
	
	return Vout;
};


float PS(VertexOut pin) : SV_Depth
{
	// Nothing
	float3 LightPos = ViewPos;
	
	// Get distance between the shading point and light source
	float LightDistance = length(pin.WorldPos - LightPos);
	
	// Map to [0,1] range by dividing by far plane
	LightDistance /= 10.0f;
		
	// Write this as modified depth
	return LightDistance;
}
