#include "Particle.hlsli"

PSIn main(VSIn vin)
{
	PSIn vout;
	float4x4 projView = mul(proj, view);
	vout.PosH = mul(projView, float4(vin.Pos, 1.0f));
	vout.Lifespan = vin.Lifespan;
	vout.TexCoords = vin.TexCoords;
	return vout;
}
