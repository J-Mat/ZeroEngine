#include "../Common.hlsl"

VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));
	Vout.WorldPos = Vout.PosH.xyz;

	Vout.PosH = mul(View, Vout.PosH);
	Vout.PosH = mul(Projection, Vout.PosH);

	Vout.Normal = mul((float3x3)Model ,vin.Normal);	
	return Vout;
};


PixelOutput PS(VertexOut pin) 
{
	PixelOutput Out;
    
	float3 Color = pin.Normal * 0.5 + 0.5; 
	Out.BaseColor = float4(Color,1.0f);
	return Out;
}
