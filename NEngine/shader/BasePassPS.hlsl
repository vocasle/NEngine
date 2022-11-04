#include "BasePass.hlsli"

#define NENGINE_DRAW_BASE_PASS_NORMAL_MAP 1

float4 main(PSIn pin) : SV_TARGET
{
    const float2 normCoord = float2(pin.PosW.w, pin.NormalW.w);

    float4 vNt = normalTex.Sample(normalSam, normCoord);
    float3 vT = pin.TangentW.xyz;
    float3 vN = pin.NormalW.xyz;
    float3 vB = cross(vN, vT) * pin.TangentW.w;
    float3 vNout = normalize(vNt.x * vT + vNt.y * vB + vNt.z * vN);

#if NENGINE_DRAW_BASE_PASS_VERTEX_NORMAL
    return float4(pin.NormalW.xyz, 1);
#elif NENGINE_DRAW_BASE_PASS_VERTEX_TANGENT
    return float4(pin.TangentW.xyz, 1);
#elif NENGINE_DRAW_BASE_PASS_NORMAL_MAP
    return float4(vNout, 1);
#endif

    return material.BaseColor * baseColorTex.Sample(baseColorSam, normCoord);
}