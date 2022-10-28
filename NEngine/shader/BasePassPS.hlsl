#include "BasePass.hlsli"

float4 main(PSIn pin) : SV_TARGET
{
    return material.BaseColor;
}