#include "../Common.hlsl"


VertexOut VS(VertexIn vin)
{
	VertexOut Vout;
	
	Vout.PosH = mul(Model, float4(vin.PosL, 1.0f));

	Vout.PosH = mul(ProjectionView, Vout.PosH);
	//Vout.PosH = mul(View, Vout.PosH);
	//Vout.PosH = mul(Projection, Vout.PosH);
	
	return Vout;
};


void PS(VertexOut pin) 
{
	// Nothing
}
