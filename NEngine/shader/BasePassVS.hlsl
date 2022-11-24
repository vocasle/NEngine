#include "BasePass.hlsli"

PSIn main(VSIn vin) {
  PSIn vout = (PSIn)0;

  vout.TexCoords = vin.TexCoords;

  float4x4 pvw = mul(proj, view);
  pvw = mul(pvw, world);
  vout.PosH = mul(pvw, float4(vin.Pos, 1.0));
  vout.PosW = mul(world, float4(vin.Pos, 1.0)).xyz;

  float3 N = mul((float3x3)worldInvTranspose, vin.Normal.xyz);
  float3 T = mul((float3x3)worldInvTranspose, vin.Tangent.xyz);
  vout.BitangentW = vin.Tangent.w * cross(N, T);

  vout.NormalW = N;
  vout.TangentW = T;

  return vout;
}