#include "Common.hlsli"

VSOut main(VSIn vin)
{
	VSOut vout = (VSOut)0;
    float4x4 pvw = mul(proj, view);
    pvw = mul(pvw, world);
    vout.PosH = mul(pvw, float4(vin.Pos.xyz, 1.0f));
	return vout;
}
