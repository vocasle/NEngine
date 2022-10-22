#include "Common.hlsli"

VSOut main(VSIn In)
{
	VSOut Out;
	float4x4 pvw = mul(proj, view);
	pvw = mul(pvw, world);
	Out.PosH = mul(pvw, float4(In.Pos.xyz, 1.0f));
	// TODO: Check matrix inverse code. It generates invalid matrix
	Out.NormalW = float4(mul(world, float4(In.Normal.xyz, 0.0f)).xyz, In.Normal.w);
	Out.PosW = float4(mul(world, float4(In.Pos.xyz, 1.0f)).xyz, In.Pos.w);
	Out.ShadowPosH = mul(shadowTransform, float4(In.Pos.xyz, 1.0f));
	Out.TangentW = float4(mul(world, float4(In.Tangent.xyz, 0.0f)).xyz, In.Tangent.w);
	return Out;
}
