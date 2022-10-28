#include "BasePass.hlsli"

PSIn main(VSIn vin)
{
    PSIn vout = (PSIn)0;

    float4x4 pvw = mul(proj, view);
	pvw = mul(pvw, world);
	vout.PosH = mul(pvw, float4(vin.Pos.xyz, 1.0f));
	vout.NormalW = float4(mul(world, float4(vin.Normal.xyz, 0.0f)).xyz, vin.Normal.w);
	vout.PosW = float4(mul(world, float4(vin.Pos.xyz, 1.0f)).xyz, vin.Pos.w);
	vout.TangentW = float4(mul(world, float4(vin.Tangent.xyz, 0.0f)).xyz, vin.Tangent.w);
    
    return vout;
}