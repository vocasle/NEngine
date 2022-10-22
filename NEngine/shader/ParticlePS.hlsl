#include "Particle.hlsli"

float4 main(PSIn pin) : SV_TARGET
{
	float4 sampled = diffuseTexture.Sample(defaultSampler, pin.TexCoords) * color;
	return sampled;
}
