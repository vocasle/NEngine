#include "BasePass.hlsli"

float4 main(PSIn pin) : SV_TARGET
{
    return material.BaseColor * baseColorTex.Sample(baseColorSam, float2(pin.PosW.w, pin.NormalW.w));
}